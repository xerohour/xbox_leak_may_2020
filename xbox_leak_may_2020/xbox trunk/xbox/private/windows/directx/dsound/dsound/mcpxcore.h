/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mcpxcore.h
 *  Content:    MCP-X audio device objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  01/09/01    dereks  Created.
 *
 ****************************************************************************/

#ifndef __MCPXCORE_H__
#define __MCPXCORE_H__

// #define MCPX_LOG_APU_WRITES
// #define MCPX_LOG_APU_READS
// #define MCPX_LOG_VP_WRITES
// #define MCPX_LOG_VOICE_STRUCT_READS
// #define MCPX_LOG_VOICE_STRUCT_WRITES

#define MCPX_DEBUG_STUCK_VOICES
// #define MCPX_NO_VOICE_OFF
#define MCPX_HANDLE_DELTA_PANICS_FROM_ISR
// #define MCPX_ENABLE_DELTA_WARNINGS
// #define MCPX_SIMULATE_STUCK_VOICE
#define MCPX_NO_OVERLAP_MIXBINS

#ifdef DEBUG
// #define MCPX_ENABLE_ISR_DEBUGGING
#endif // DEBUG

#include "mcpxhw.h"

DEFINETYPE(MCPX_VOICE_HANDLE, WORD);

#define MCPX_VOICE_HANDLE_INVALID ((MCPX_VOICE_HANDLE)~0UL)

#ifdef __cplusplus

//
// Register value macros
//

#define MCPX_REF_SHIFT(drf) \
    (0 ? drf)

#define MCPX_REF_MASK(drf) \
    ((1 << ((1 ? drf) - (0 ? drf) + 1)) - 1)

#define MCPX_CLEAR_REG_VALUE(arg, drf) \
    ((arg) & ~(MCPX_REF_MASK(drf) << MCPX_REF_SHIFT(drf)))

#define MCPX_MAKE_REG_VALUE(val, drf) \
    ((((DWORD)(val)) & MCPX_REF_MASK(drf)) << MCPX_REF_SHIFT(drf))

#define MCPX_GET_REG_VALUE(arg, drf) \
    (((arg) >> MCPX_REF_SHIFT(drf)) & MCPX_REF_MASK(drf))

#define MCPX_SET_REG_VALUE(arg, val, drf) \
    (MCPX_CLEAR_REG_VALUE(arg, drf) | MCPX_MAKE_REG_VALUE(val, drf))

#define MCPX_DEVICE_BASE(d) \
    (0 ? d)

//
// APU core register access macros
//

#ifdef MCPX_LOG_APU_WRITES
#define DPF_MCPX_APU_WRITE DPF_ABSOLUTE
#else // MCPX_LOG_APU_WRITES
#define DPF_MCPX_APU_WRITE()
#endif // MCPX_LOG_APU_WRITES

#ifdef MCPX_LOG_APU_READS
#define DPF_MCPX_APU_READ DPF_ABSOLUTE
#else // MCPX_LOG_APU_READS
#define DPF_MCPX_APU_READ()
#endif // MCPX_LOG_APU_READS

#define MCPX_REGISTER_ADDRESS(reg) \
    (XPCICFG_APU_MEMORY_REGISTER_BASE_0 + (reg))

#define MCPX_REGISTER(reg) \
    (*(volatile DWORD *)MCPX_REGISTER_ADDRESS(reg))

#define MCPX_REG_WRITE(reg, value) \
    { \
        DPF_MCPX_APU_WRITE("APU write: offset %x (%x), value %x", (DWORD)(reg), MCPX_REGISTER_ADDRESS(reg), (DWORD)(value)); \
        MCPX_REGISTER(reg) = (DWORD)(value); \
    }

#define MCPX_REG_READ(reg, value) \
    { \
        *(value) = MCPX_REGISTER(reg); \
        DPF_MCPX_APU_READ("APU read: offset %x (%x), value %x", (DWORD)(reg), MCPX_REGISTER_ADDRESS(reg), (DWORD)*(value)); \
    }

//
// VP register access macros
//

#ifdef MCPX_LOG_VP_WRITES
#define DPF_MCPX_VP_WRITE DPF_ABSOLUTE
#else // MCPX_LOG_VP_WRITES
#define DPF_MCPX_VP_WRITE()
#endif // MCPX_LOG_VP_WRITES

#define MCPX_VOICE_PROCESSOR_BASE \
    MCPX_REGISTER_ADDRESS(MCPX_DEVICE_BASE(NV_PAPU_NV1BA0))

#define MCPX_VOICE_REGISTER(reg) \
    (((LPMCPXVPREGSET)MCPX_VOICE_PROCESSOR_BASE)->reg)

#define MCPX_CHECK_VOICE_FIFO_INIT(method_count) \
    { \
        ASSERT((method_count) <= PIO_METHOD_QUEUE_CLEAR); \
        while(MCPX_VOICE_REGISTER(PIOFree) / 4 < (DWORD)(method_count)) \
        { \
            DPF_MCPX_VP_WRITE("Waiting for PIO to free up..."); \
        } \
    }

#define MCPX_CHECK_VOICE_FIFO(method_count) \
    { \
        ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL); \
        MCPX_CHECK_VOICE_FIFO_INIT(method_count); \
    }

#define MCPX_VOICE_WRITE(reg, value) \
    { \
        DPF_MCPX_VP_WRITE("VP write: " QUOTE1(reg) " (offset %x, address %x), value %x", offsetof(MCPXVPREGSET, reg), &MCPX_VOICE_REGISTER(reg), (DWORD)(value)); \
        MCPX_VOICE_REGISTER(reg) = (DWORD)(value); \
    }

//
// Voice data access macros
//

#ifdef MCPX_LOG_VOICE_STRUCT_READS
#define DPF_MCPX_VOICE_STRUCT_READ DPF_ABSOLUTE
#else // MCPX_LOG_VOICE_STRUCT_READS
#define DPF_MCPX_VOICE_STRUCT_READ()
#endif // MCPX_LOG_VOICE_STRUCT_READS

#ifdef MCPX_LOG_VOICE_STRUCT_WRITES
#define DPF_MCPX_VOICE_STRUCT_WRITE DPF_ABSOLUTE
#else // MCPX_LOG_VOICE_STRUCT_WRITES
#define DPF_MCPX_VOICE_STRUCT_WRITE()
#endif // MCPX_LOG_VOICE_STRUCT_WRITES

#define MCPX_VOICE_STRUCT_BASE(voice_index) \
    ((DWORD)(CMcpxCore::m_ctxMemory[MCPX_MEM_VOICE].VirtualAddress) + ((DWORD)(voice_index) * NV_PAVS_SIZE))

#define MCPX_VOICE_STRUCT_OFFSET(voice_index, reg) \
    (MCPX_VOICE_STRUCT_BASE(voice_index) + (reg))

#define MCPX_VOICE_STRUCT(voice_index, reg) \
    (*(volatile DWORD *)MCPX_VOICE_STRUCT_OFFSET(voice_index, reg))

#define MCPX_VOICE_STRUCT_READ(voice_index, reg, value) \
    { \
        *(value) = MCPX_VOICE_STRUCT(voice_index, reg); \
        DPF_MCPX_VOICE_STRUCT_READ("Voice struct read: voice %x, reg %x, address %x, value %x", (DWORD)(voice_index), (DWORD)(reg), MCPX_VOICE_STRUCT_OFFSET(voice_index, reg), (DWORD)*(value)); \
    }

#define MCPX_VOICE_STRUCT_WRITE(voice_index, reg, value) \
    { \
        MCPX_VOICE_STRUCT(voice_index, reg) = (DWORD)(value); \
        DPF_MCPX_VOICE_STRUCT_WRITE("Voice struct write: voice %x, reg %x, address %x, value %x", (DWORD)(voice_index), (DWORD)(reg), MCPX_VOICE_STRUCT_OFFSET(voice_index, reg), (DWORD)(value)); \
    }

// 
// MCP-X physical resource descriptor
//

BEGIN_DEFINE_STRUCT()
	PVOID   VirtualAddress;
	ULONG   PhysicalAddress;
	ULONG   Size;
#ifdef ENABLE_SLOP_MEMORY_RECOVERY
    BOOL    fOwned;
#endif // ENABLE_SLOP_MEMORY_RECOVERY
END_DEFINE_STRUCT(MCPX_ALLOC_CONTEXT);

//
// GP output buffers
//

BEGIN_DEFINE_ENUM()
    MCPX_GPOUTPUT_BOOTSOUND = 0,
    MCPX_GPOUTPUT_COUNT
END_DEFINE_ENUM_();

//
// EP output buffers
//

BEGIN_DEFINE_ENUM()
    MCPX_EPOUTPUT_AC97_ANALOG = 0,
    MCPX_EPOUTPUT_AC97_DIGITAL,
    MCPX_EPOUTPUT_COUNT
END_DEFINE_ENUM_();

//
// Front-end states
//

BEGIN_DEFINE_ENUM()
    MCPX_FE_STATE_HALTED,
    MCPX_FE_STATE_FREE_RUNNING,
    MCPX_FE_STATE_ISO,
    MCPX_FE_STATE_NON_ISO,
END_DEFINE_ENUM(MCPX_FE_STATE);

//
// Setup engine states
//

BEGIN_DEFINE_ENUM()
    MCPX_SE_STATE_OFF = 0,
    MCPX_SE_STATE_AC_SYNC,
    MCPX_SE_STATE_SW,
    MCPX_SE_STATE_FREE_RUNNING,
    MCPX_SE_STATE_ISO,
    MCPX_SE_STATE_NON_ISO,
END_DEFINE_ENUM(MCPX_SE_STATE);

#define MCPX_SE_STATE_DEFAULT MCPX_SE_STATE_AC_SYNC

//
// APU memory.  The order actually matters, so don't mess with it
//

BEGIN_DEFINE_ENUM()
    MCPX_MEM_GPOUTPUT = 0,
    MCPX_MEM_EPOUTPUT,
    MCPX_MEM_MAGICWRITE,
    MCPX_MEM_VOICE,
    MCPX_MEM_NOTIFIERS,
    MCPX_MEM_INPUTSGE,
    MCPX_MEM_INPUTPRD,
    MCPX_MEM_HRTFTARGET,
    MCPX_MEM_HRTFCURRENT,
    MCPX_MEM_GPOUTPUTSGE,    
    MCPX_MEM_EPOUTPUTSGE,
    MCPX_MEM_GPMULTIPASS,
    MCPX_MEM_GPSCRATCH,
    MCPX_MEM_GPSCRATCHSGE,
    MCPX_MEM_EPSCRATCH,
    MCPX_MEM_EPSCRATCHSGE,
    MCPX_MEM_COUNT,
END_DEFINE_ENUM_();

//
// Voice list identifiers.  Add 1 to this value to get the hardware register 
// equivalent.
//

BEGIN_DEFINE_ENUM()
    MCPX_VOICELIST_2D = 0,
    MCPX_VOICELIST_3D,
    MCPX_VOICELIST_MP,
    MCPX_VOICELIST_COUNT,
    MCPX_VOICELIST_INVALID = 0xFF
END_DEFINE_ENUM_();

//
// Voice notifiers
//

#define MCPX_NOTIFIER_BASE_OFFSET 2
                                        
BEGIN_DEFINE_ENUM()
    MCPX_NOTIFIER_SSLA_DONE = 0,
    MCPX_NOTIFIER_SSLB_DONE,
    MCPX_NOTIFIER_SSLA_GETPOS,
    MCPX_NOTIFIER_VOICE_OFF,
    MCPX_NOTIFIER_COUNT
END_DEFINE_ENUM_();

//
// Forward declarations
//

namespace DirectSound
{
    class CDirectSoundSettings;
    class CDirectSoundVoiceSettings;
    class CDirectSoundBufferSettings;
    class CDirectSoundStreamSettings;
    class CMcpxGPDspManager;
    class CMcpxEPDspManager;
}

//
// The MCPX APU core object
//

namespace DirectSound
{
    class CMcpxCore
    {
    public:
        static const DWORD          m_adwGPOutputBufferSizes[MCPX_GPOUTPUT_COUNT];  // GP Output buffer sizes, in bytes
        static const DWORD          m_adwEPOutputBufferSizes[MCPX_EPOUTPUT_COUNT];  // EP Output buffer sizes, in bytes
	    static MCPX_ALLOC_CONTEXT   m_ctxMemory[MCPX_MEM_COUNT];                    // APU memory
        CDirectSoundSettings *      m_pSettings;                                    // Shared settings object

    protected:                      

#ifdef ENABLE_SLOP_MEMORY_RECOVERY
    
        CMcpxSlopMemoryHeap *       m_pSlopMemoryHeap;                              // Slop memory heap

#endif // ENABLE_SLOP_MEMORY_RECOVERY

        CMcpxGPDspManager *         m_pGpDspManager;                                // GP DSP manager object

#ifndef MCPX_BOOT_LIB

        CMcpxEPDspManager *         m_pEpDspManager;                                // EP DSP manager object

#endif // MCPX_BOOT_LIB

        CAc97Device                 m_Ac97;                                         // AC97 device object

    public:
        CMcpxCore(CDirectSoundSettings *pSettings);
        virtual ~CMcpxCore(void);

    public:
        // Initialization
        HRESULT Initialize(void);

        // APU component state
        BOOL IdleVoiceProcessor(BOOL fIdle);

        // Utility functions
        static DWORD GetPhysicalMemoryProperties(LPVOID pvBuffer, DWORD dwBufferSize, LPDWORD pdwPageOffset, LPDWORD pdwContiguousLength);
        static DWORD MapTransfer(LPVOID *ppvBuffer, LPDWORD pdwBufferSize, LPDWORD pdwBytesMapped);
        static HRESULT AllocateContext(PMCPX_ALLOC_CONTEXT pContext, DWORD dwAlignment, DWORD dwFlags);

    protected:
        // Shutdown
        void Reset(void);

        // APU component state
        void SetInterruptState(BOOL fEnabled);
        void SetPrivLockState(BOOL fLocked);
        void SetFrontEndState(MCPX_FE_STATE nState);
        void SetSetupEngineState(MCPX_SE_STATE nState);
        void ResetGlobalCounters(void);

        // APU setup
        HRESULT AllocateApuMemory(void);
        void SetupFrontEndProcessor(void);
        void SetupVoiceProcessor(void);
        void SetupDSPs(void);
        void SetupGlobalProcessor(void);

#ifndef MCPX_BOOT_LIB

        void SetupEncodeProcessor(void);

#endif // MCPX_BOOT_LIB

        HRESULT SetupAc97(void);
    };

    __inline void CMcpxCore::SetupDSPs(void)
    {
        SetupGlobalProcessor();

#ifndef MCPX_BOOT_LIB

        SetupEncodeProcessor();
        KeStallExecutionProcessor(10 * 1000);

#endif // MCPX_BOOT_LIB

    }
}

//
// MCPX notifier wrapper class
//

namespace DirectSound
{
    class CMcpxNotifier
    {
    private:
        PMCPX_HW_NOTIFICATION       m_paNotifier;       // Base notifier pointer
        DWORD                       m_dwNotifierCount;  // Count of notifiers controlled by this object

    public:
        CMcpxNotifier(void);

    public:
        // Initialization
        void Initialize(DWORD dwBaseNotifierIndex, DWORD dwNotifierCount);
        void Free(void);

        // Notifier status
        BOOL GetStatus(DWORD dwNotifierIndex);
        void SetStatus(DWORD dwNotifierIndex, BOOL fSignaled);
        void Reset(void);
    };

    __inline CMcpxNotifier::CMcpxNotifier(void)
    {
        Free();
    }

    __inline void CMcpxNotifier::Initialize(DWORD dwBaseNotifierIndex, DWORD dwNotifierCount)
    {
        ASSERT(!m_paNotifier);
        ASSERT(!m_dwNotifierCount);

        m_paNotifier = (PMCPX_HW_NOTIFICATION)CMcpxCore::m_ctxMemory[MCPX_MEM_NOTIFIERS].VirtualAddress + dwBaseNotifierIndex;
        m_dwNotifierCount = dwNotifierCount;

        Reset();
    }

    __inline void CMcpxNotifier::Free(void)
    {
        m_paNotifier = NULL;
        m_dwNotifierCount = 0;
    }

    __inline BOOL CMcpxNotifier::GetStatus(DWORD dwNotifierIndex)
    {
        ASSERT(m_paNotifier);
        ASSERT(dwNotifierIndex < m_dwNotifierCount);
    
        switch(m_paNotifier[dwNotifierIndex].Status)
        {
            case NV1BA0_NOTIFICATION_STATUS_DONE_SUCCESS:
                return TRUE;

            case NV1BA0_NOTIFICATION_STATUS_IN_PROGRESS:
                return FALSE;

            default:
                ASSERTMSG("Unexpected notifier status");
                return FALSE;
        }
    }

    __inline void CMcpxNotifier::SetStatus(DWORD dwNotifierIndex, BOOL fSignaled)
    {
        ASSERT(m_paNotifier);
        ASSERT(dwNotifierIndex < m_dwNotifierCount);

        m_paNotifier[dwNotifierIndex].Status = fSignaled ? NV1BA0_NOTIFICATION_STATUS_DONE_SUCCESS : NV1BA0_NOTIFICATION_STATUS_IN_PROGRESS;
    }

    __inline void CMcpxNotifier::Reset(void)
    {
        DWORD                   dwNotifierIndex;
    
        ASSERT(m_paNotifier);
        ASSERT(m_dwNotifierCount);

        for(dwNotifierIndex = 0; dwNotifierIndex < m_dwNotifierCount; dwNotifierIndex++)
        {
            m_paNotifier[dwNotifierIndex].Status = NV1BA0_NOTIFICATION_STATUS_IN_PROGRESS;
        }
    }
}

//
// Miscelaneous MCPX includes
//

#include "dspdma.h"
#include "gpdsp.h"
#include "epdsp.h"
#include "mcpapu.h"
#include "mcpvoice.h"
#include "mcpbuf.h"
#include "mcpstrm.h"

#endif // __cplusplus

#endif // __MCPXCORE_H__
