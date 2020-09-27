/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       ac97.h
 *  Content:    DirectSound AC97 device driver.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  09/27/00    dereks  Created.
 *
 ****************************************************************************/

#ifndef __AC97_H__
#define __AC97_H__

#ifndef AC97_AMR
#define AC97_USE_MIO
#endif // AC97_AMR

#include "ac97reg.h"
#include "acireg.h"
#include <conio.h>

#pragma intrinsic(_inp,_inpw,_inpd,_outp,_outpw,_outpd)

//
// AC97 channel identifiers
//

typedef enum
{
    AC97_CHANNELTYPE_ANALOG = 0,
    AC97_CHANNELTYPE_DIGITAL,
    AC97_CHANNELTYPE_COUNT
} AC97CHANNELTYPE;

//
// AC97 channel interrupt bits
//

#define AC97_CHANNELINTERRUPT(line) \
    (1UL << (line))

#define AC97_CHANNELINTERRUPT_ANALOG    AC97_CHANNELINTERRUPT(AC97_CHANNELTYPE_ANALOG)
#define AC97_CHANNELINTERRUPT_DIGITAL   AC97_CHANNELINTERRUPT(AC97_CHANNELTYPE_DIGITAL)

#define AC97_CHANNELINTERRUPT_MASK      (AC97_CHANNELINTERRUPT(AC97_CHANNELTYPE_COUNT) - 1)

//
// AC97 channel interrupt callback
//

typedef void (CALLBACK *LPFNAC97CHANNELCALLBACK)(LPVOID pvContext);

//
// AC97 device object creation flags
//

#define AC97_OBJECTF_DIRECTISR      0x00000001      // The device does not spawn a DPC to handle interrupts

//
// AC97 channel object creation flags
//

#define AC97CHANNEL_OBJECTF_STREAM          0x00000001      // The channel removed played packets from the list
#define AC97CHANNEL_OBJECTF_DISCONTINUITY   0x00000002      // Internal channel flag.  Do not set

#ifdef __cplusplus

namespace DirectSound
{
    //
    // Forward declarations
    //

    class CAc97Channel;

    //
    // AC97 device object
    //

    class CAc97Device
    {
        friend class CAc97Channel;

    public:
        static const WAVEFORMATEX   m_wfxFormat;                                            // Device format
                                                                                        
    #ifdef AC97_AMR                                                                         
                                                                                        
        static const DWORD          m_dwVendorId;                                           // PCI device vendor identifier
        static const DWORD          m_dwDeviceId;                                           // PCI device identifier
        static const DWORD          m_dwBusNumber;                                          // PCI bus index
        static const DWORD          m_dwDeviceNumber;                                       // PCI device index
        static const DWORD          m_dwFunctionNumber;                                     // PCI device function index
                                                                                        
    #endif // AC97_AMR                                                                      
                                                                                        
        static const DWORD          m_dwAc97RegisterBase;                                   // AC97 register base address
        static const DWORD          m_dwAciRegisterBase;                                    // ACI register base address
        static CAc97Device *        m_pDevice;                                              // The one-and-only device object
        CAc97Channel *              m_apChannels[AC97_CHANNELTYPE_COUNT];                   // Channel objects
                                                                                        
    protected:                                                                              
        DWORD                       m_dwFlags;                                              // Object creation flags
        static KINTERRUPT           m_Interrupt;                                            // Interrupt object
        HAL_SHUTDOWN_REGISTRATION   m_HalShutdownData;                                      // HAL shutdown registration data
        KDPC                        m_dpc;                                                  // Interrupt DPC routine
        BYTE                        m_abPendingBufferCompletions[AC97_CHANNELTYPE_COUNT][2];// Count of buffer completions waiting to be handled (live and cached)

    public:
        CAc97Device(void);
        ~CAc97Device(void);

    public:
        CAc97Device &operator=(const CAc97Device &);

    public:
        // Initialization
        HRESULT Initialize(DWORD dwFlags = 0);
        void Terminate(void);

        // Channel allocation
        HRESULT CreateChannel(AC97CHANNELTYPE nChannelType, CAc97Channel **ppChannel = NULL);
        void ReleaseChannel(AC97CHANNELTYPE nChannelType);
        void ReleaseChannel(CAc97Channel *pChannel);

    protected:
        // CODEC power state
        BOOL CodecReady(void);
        HRESULT PowerUp(void);

        // Interrupt handlers
        DWORD ServiceAciInterrupt(void);
        void ServiceAciInterruptDpc(void);
        void SynchronizeAciInterrupt(void);
        DWORD GetInterruptStatus(void);

        // Register access
        static BOOL AcquireCodecSemaphore(void);
    
        static BYTE PeekRegister8(DWORD dwRegister);
        static void PokeRegister8(DWORD dwRegister, BYTE bValue);
        static WORD PeekRegister16(DWORD dwRegister);
        static void PokeRegister16(DWORD dwRegister, WORD wValue);
        static DWORD PeekRegister32(DWORD dwRegister);
        static void PokeRegister32(DWORD dwRegister, DWORD dwValue);

        static BYTE PeekAciRegister8(DWORD dwRegister);
        static void PokeAciRegister8(DWORD dwRegister, BYTE bValue);
        static WORD PeekAciRegister16(DWORD dwRegister);
        static void PokeAciRegister16(DWORD dwRegister, WORD wValue);
        static DWORD PeekAciRegister32(DWORD dwRegister);
        static void PokeAciRegister32(DWORD dwRegister, DWORD dwValue);

        static BOOL PeekAc97Register(AC97REGISTER reg, LPWORD pwValue);
        static BOOL PokeAc97Register(AC97REGISTER reg, WORD wValue);
        static BOOL VerifyPokeAc97Register(AC97REGISTER reg, WORD wValue);

        static void WaitRegisterRetry(void);

    private:
        // HAL callbacks
        static BOOLEAN AciInterruptServiceRoutine(PKINTERRUPT pInterrupt, LPVOID pvContext);
        static void AciInterruptDpcHandler(PKDPC pdpc, LPVOID pvDeferredContext, LPVOID pvSystemArgument1, LPVOID pvSystemArgument2);
        static void AciShutdownNotifier(PHAL_SHUTDOWN_REGISTRATION pHalShutdownData);
        static BOOLEAN AciSynchronizationRoutine(LPVOID pvContext);
    };

    __inline BYTE CAc97Device::PeekRegister8(DWORD dwRegister)
    {

    #ifdef AC97_USE_MIO

        return *(LPBYTE)dwRegister;

    #else // AC97_USE_MIO

        return (BYTE)_inp((WORD)dwRegister);

    #endif // AC97_USE_MIO

    }    

    __inline void CAc97Device::PokeRegister8(DWORD dwRegister, BYTE bValue)
    {

    #ifdef AC97_USE_MIO

        *(LPBYTE)dwRegister = bValue;

    #else // AC97_USE_MIO

        _outp((WORD)dwRegister, bValue);

    #endif // AC97_USE_MIO

    }

    __inline WORD CAc97Device::PeekRegister16(DWORD dwRegister)
    {

    #ifdef AC97_USE_MIO

        return *(LPWORD)dwRegister;

    #else // AC97_USE_MIO

        return _inpw((WORD)dwRegister);

    #endif // AC97_USE_MIO

    }

    __inline void CAc97Device::PokeRegister16(DWORD dwRegister, WORD wValue)
    {

    #ifdef AC97_USE_MIO

        *(LPWORD)dwRegister = wValue;

    #else // AC97_USE_MIO

        _outpw((WORD)dwRegister, wValue);

    #endif // AC97_USE_MIO

    }

    __inline DWORD CAc97Device::PeekRegister32(DWORD dwRegister)
    {

    #ifdef AC97_USE_MIO

        return *(LPDWORD)dwRegister;

    #else // AC97_USE_MIO

        return _inpd((WORD)dwRegister);

    #endif // AC97_USE_MIO

    }

    __inline void CAc97Device::PokeRegister32(DWORD dwRegister, DWORD dwValue)
    {

    #ifdef AC97_USE_MIO

        *(LPDWORD)dwRegister = dwValue;

    #else // AC97_USE_MIO

        _outpd((WORD)dwRegister, dwValue);

    #endif // AC97_USE_MIO

    }

    __inline BYTE CAc97Device::PeekAciRegister8(DWORD dwRegister)
    {
        return PeekRegister8(m_dwAciRegisterBase + dwRegister);
    }
    
    __inline void CAc97Device::PokeAciRegister8(DWORD dwRegister, BYTE bValue)
    {
        PokeRegister8(m_dwAciRegisterBase + dwRegister, bValue);
    }

    __inline WORD CAc97Device::PeekAciRegister16(DWORD dwRegister)
    {
        return PeekRegister16(m_dwAciRegisterBase + dwRegister);
    }

    __inline void CAc97Device::PokeAciRegister16(DWORD dwRegister, WORD wValue)
    {
        PokeRegister16(m_dwAciRegisterBase + dwRegister, wValue);
    }

    __inline DWORD CAc97Device::PeekAciRegister32(DWORD dwRegister)
    {
        return PeekRegister32(m_dwAciRegisterBase + dwRegister);
    }

    __inline void CAc97Device::PokeAciRegister32(DWORD dwRegister, DWORD dwValue)
    {
        PokeRegister32(m_dwAciRegisterBase + dwRegister, dwValue);
    }

    __inline void CAc97Device::WaitRegisterRetry(void)
    {
        KeStallExecutionProcessor(20);
    }

    __inline BOOLEAN CAc97Device::AciInterruptServiceRoutine(PKINTERRUPT pInterrupt, LPVOID pvContext)
    {
        CAc97Device *           pThis   = (CAc97Device *)pvContext;
    
        return (BOOLEAN)MAKEBOOL(pThis->ServiceAciInterrupt());
    }

    __inline void CAc97Device::AciInterruptDpcHandler(PKDPC pdpc, LPVOID pvDeferredContext, LPVOID pvSystemArgument1, LPVOID pvSystemArgument2)
    {
        CAc97Device *           pThis   = (CAc97Device *)pvDeferredContext;
    
        pThis->ServiceAciInterruptDpc();
    }

    __inline void CAc97Device::AciShutdownNotifier(PHAL_SHUTDOWN_REGISTRATION pHalShutdownData)
    {
        CAc97Device *           pThis   = CONTAINING_RECORD(pHalShutdownData, CAc97Device, m_HalShutdownData);
    
        pThis->Terminate();
    }

    __inline BOOLEAN CAc97Device::AciSynchronizationRoutine(LPVOID pvContext)
    {
        CAc97Device *           pThis   = (CAc97Device *)pvContext;
    
        pThis->SynchronizeAciInterrupt();

        return TRUE;
    }    

    //
    // AC97 channel object
    //

    class CAc97Channel
    {
        friend class CAc97Device;

    public:
        static const DWORD          m_adwRegisterOffsets[AC97_CHANNELTYPE_COUNT];   // Channel register offsets
        static const DWORD          m_dwSilenceSize;                                // Silent packet size

    public:
        const AC97CHANNELTYPE       m_nChannelType;                                 // Channel type
        DWORD                       m_dwPosition;                                   // Absolute channel position
                                                                                
    protected:                                                                      
        CAc97Device *               m_pDevice;                                      // Pointer back to the parent device
        DWORD                       m_dwFlags;                                      // Object creation flags
        LPFNAC97CHANNELCALLBACK     m_pfnCallback;                                  // Interrupt callback function
        LPVOID                      m_pvCallbackContext;                            // Callback function context
        LPACIPRD                    m_pPrdl;                                        // Physical resource descriptor list
        DWORD                       m_dwPrdlAddress;                                // PRDL physical address
        LPVOID                      m_pvSilence;                                    // Silent packet data
        BYTE                        m_bPrdCount;                                    // Count of PRDs in the PRDL
        BYTE                        m_bCurrentPrd;                                  // Current index into the PRDL
        DWORD                       m_dwMode;                                       // Channel mode
        DWORD                       m_dwLastPosition;                               // Last position returned from GetPosition

    public:
        CAc97Channel(CAc97Device *pDevice, AC97CHANNELTYPE nChannelType);
        ~CAc97Channel(void);

    public:
        CAc97Channel &operator=(const CAc97Channel &);

    public:
        // Initialization
        HRESULT Initialize(DWORD dwFlags = 0, LPFNAC97CHANNELCALLBACK pfnCallback = NULL, LPVOID pvContext = NULL);
        void Terminate(void);

        // Reference counting
        void Release(void);

        // Audio data packets
        DWORD GetFreePacketCount(void);
        void AttachPacket(LPCVOID pvBufferData, DWORD dwBufferSize);
        void AttachPacket(DWORD dwBufferAddress, DWORD dwBufferSize);
        void AttachBuffer(LPCVOID pvBufferData, DWORD dwBufferSize, DWORD dwPacketCount);
        void AttachBuffer(DWORD dwBufferAddress, DWORD dwBufferSize, DWORD dwPacketCount);

        // Channel state
        void Run(DWORD dwPosition = -1);
        void Pause(void);
        void Flush(BOOL fResetPosition = TRUE);
        void Discontinuity(void);
        void Reset(void);
    
        // Channel position
        DWORD GetPosition(void);

        // Channel mode
        void SetMode(DWORD dwMode);

    protected:
        // Channel position
        void SetPosition(DWORD dwPosition);

        // Interrupts
        void ServiceInterrupt(void);

        // Resource descriptor list
        void SetPrdIndeces(BYTE bCurrentPrd, BYTE bPrdCount, BOOL fApplyCurrent, BOOL fApplyLast);

        // Register access
        BYTE PeekAciRegister8(DWORD dwRegister);
        void PokeAciRegister8(DWORD dwRegister, BYTE bValue);
        WORD PeekAciRegister16(DWORD dwRegister);
        void PokeAciRegister16(DWORD dwRegister, WORD wValue);
        DWORD PeekAciRegister32(DWORD dwRegister);
        void PokeAciRegister32(DWORD dwRegister, DWORD dwValue);
    };

    __inline void CAc97Device::ReleaseChannel(CAc97Channel *pChannel)
    {
        ReleaseChannel(pChannel->m_nChannelType);
    }

    __inline void CAc97Channel::Release(void)
    {
        m_pDevice->ReleaseChannel(this);
    }

    __inline void CAc97Channel::AttachPacket(LPCVOID pvBufferData, DWORD dwBufferSize)
    {
        AttachPacket(MmGetPhysicalAddress((LPVOID)pvBufferData), dwBufferSize);
    }

    __inline void CAc97Channel::AttachBuffer(LPCVOID pvBufferData, DWORD dwBufferSize, DWORD dwPacketCount)
    {
        AttachBuffer(MmGetPhysicalAddress((LPVOID)pvBufferData), dwBufferSize, dwPacketCount);
    }

    __inline DWORD CAc97Channel::GetFreePacketCount(void)
    {
        return ACIPRDL_ENTRY_COUNT - m_bPrdCount;
    }

    __inline BYTE CAc97Channel::PeekAciRegister8(DWORD dwRegister)
    {
        return m_pDevice->PeekAciRegister8(dwRegister + m_adwRegisterOffsets[m_nChannelType]);
    }

    __inline void CAc97Channel::PokeAciRegister8(DWORD dwRegister, BYTE bValue)
    {
        m_pDevice->PokeAciRegister8(dwRegister + m_adwRegisterOffsets[m_nChannelType], bValue);
    }

    __inline WORD CAc97Channel::PeekAciRegister16(DWORD dwRegister)
    {
        return m_pDevice->PeekAciRegister16(dwRegister + m_adwRegisterOffsets[m_nChannelType]);
    }

    __inline void CAc97Channel::PokeAciRegister16(DWORD dwRegister, WORD wValue)
    {
        m_pDevice->PokeAciRegister16(dwRegister + m_adwRegisterOffsets[m_nChannelType], wValue);
    }

    __inline DWORD CAc97Channel::PeekAciRegister32(DWORD dwRegister)
    {
        return m_pDevice->PeekAciRegister32(dwRegister + m_adwRegisterOffsets[m_nChannelType]);
    }

    __inline void CAc97Channel::PokeAciRegister32(DWORD dwRegister, DWORD dwValue)
    {
        m_pDevice->PokeAciRegister32(dwRegister + m_adwRegisterOffsets[m_nChannelType], dwValue);
    }
}
    
#endif // __cplusplus

#endif // __AC97_H__
