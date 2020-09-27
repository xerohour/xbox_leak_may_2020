/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       ac97.cpp
 *  Content:    DirectSound AC '97 device driver.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  09/27/00    dereks  Created based on NVidia code.
 *
 ****************************************************************************/

#include "dsoundi.h"

//#define DPF_AC97 DPF_ABSOLUTE
#define DPF_AC97()


/****************************************************************************
 *
 *  CAc97Device
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
#define DPF_FNAME "CAc97Device::CAc97Device"

#ifdef AC97_AMR

const DWORD CAc97Device::m_dwVendorId = 0x8086;
const DWORD CAc97Device::m_dwDeviceId = 0x2415;
const DWORD CAc97Device::m_dwBusNumber = 0;
const DWORD CAc97Device::m_dwDeviceNumber = 31;
const DWORD CAc97Device::m_dwFunctionNumber = 5;
const DWORD CAc97Device::m_dwAc97RegisterBase = 0xD800;
const DWORD CAc97Device::m_dwAciRegisterBase = 0xDC00;

#else // AC97_AMR

#ifdef AC97_USE_MIO

const DWORD CAc97Device::m_dwAc97RegisterBase = XPCICFG_ACI_MEMORY_REGISTER_BASE_2 + AC97_MIO_BASE;
const DWORD CAc97Device::m_dwAciRegisterBase = XPCICFG_ACI_MEMORY_REGISTER_BASE_2 + ACI_MIO_BASE;

#else // AC97_USE_MIO

const DWORD CAc97Device::m_dwAc97RegisterBase = XPCICFG_ACI_IO_REGISTER_BASE_0;
const DWORD CAc97Device::m_dwAciRegisterBase = XPCICFG_ACI_IO_REGISTER_BASE_1;

#endif // AC97_USE_MIO

#endif // AC97_AMR

const WAVEFORMATEX CAc97Device::m_wfxFormat = 
{ 
    WAVE_FORMAT_PCM,    // wFormatTag
    2,                  // nChannels
    48000,              // nSamplesPerSec
    192000,             // nAvgBytesPerSec
    4,                  // nBlockAlign
    16,                 // wBitsPerSample
    0                   // cbSize
};

CAc97Device *CAc97Device::m_pDevice = NULL;
KINTERRUPT CAc97Device::m_Interrupt = { 0 };

CAc97Device::CAc97Device
(
    void
)
{
    DPF_ENTER();

    if(m_pDevice)
    {
        DPF_ERROR("The AC97 device driver is already running");
    }

    m_pDevice = this;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CAc97Device
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
#define DPF_FNAME "CAc97Device::~CAc97Device"

CAc97Device::~CAc97Device
(
    void
)
{
    DPF_ENTER();

    Terminate();

    m_pDevice = NULL;

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
 *      DWORD [in]: flags.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Device::Initialize"

HRESULT
CAc97Device::Initialize
(
    DWORD                   dwFlags
)
{
    HRESULT                 hr                  = DS_OK;

#ifdef AC97_AMR

    DWORD                   dwRegisterValue;
    BYTE                    bInterruptLine;
    PCI_COMMON_CONFIG       PciConfig;
    PCI_SLOT_NUMBER         Slot;

#endif // AC97_AMR

    KIRQL                   Irql;
    ULONG                   ulInterruptVector;
    DWORD                   i;

    DPF_ENTER();

    //
    // Save the flags
    //
    
    m_dwFlags = dwFlags;

#ifdef AC97_AMR

    //
    // Poke the PCI controller to enable the AMR sound device
    //

    _outpd(0xCF8, 0x8000F8F0);

    dwRegisterValue = _inpd(0xCFC);

    if(dwRegisterValue & 0x200000)
    {
        _outpd(0xCFC, dwRegisterValue & ~0x200000);
    }

    //
    // Get the device's interrupt line, since it won't be in the PCI config
    //

    Slot.u.bits.DeviceNumber = m_dwDeviceNumber;
    Slot.u.bits.FunctionNumber = 0;
    Slot.u.bits.Reserved = 0;

    HalReadPCISpace(m_dwBusNumber, Slot.u.AsULONG, 0x61, &bInterruptLine, sizeof(bInterruptLine));

    //
    // Get the device's current PCI config data
    //

    Slot.u.bits.FunctionNumber = m_dwFunctionNumber;

    HalReadPCISpace(m_dwBusNumber, Slot.u.AsULONG, 0, &PciConfig, sizeof(PciConfig));

    ASSERT(m_dwVendorId == PciConfig.VendorID);
    ASSERT(m_dwDeviceId == PciConfig.DeviceID);

    //
    // Assign base addresses and IRQ
    //

    DPF_AC97("Using IRQ %x", (DWORD)bInterruptLine);

    PciConfig.Command = 5;

    PciConfig.u.type0.BaseAddresses[0] = m_dwAc97RegisterBase | 1;
    PciConfig.u.type0.BaseAddresses[1] = m_dwAciRegisterBase | 1;

    PciConfig.u.type0.InterruptLine = bInterruptLine;
    PciConfig.u.type0.InterruptPin = 0;

    HalWritePCISpace(m_dwBusNumber, Slot.u.AsULONG, 0, &PciConfig, sizeof(PciConfig));

#endif // AC97_AMR

    //
    // Make sure the CODEC is ready to be powered up
    //

    if(!CodecReady())
    {
        DPF_ERROR("CODEC not ready");
        hr = DSERR_NODRIVER;
    }
    
    //
    // Power up the CODEC
    //

    if(SUCCEEDED(hr))
    {
        hr = PowerUp();
    }

    //
    // Reset all channels
    //

    if(SUCCEEDED(hr))
    {
        for(i = 0; i < NUMELMS(CAc97Channel::m_adwRegisterOffsets); i++)
        {
            PokeAciRegister8(CAc97Channel::m_adwRegisterOffsets[i] + X_CTRL, X_CTRL_RBMR);
        }
    }

    //
    // Initialize the interrupt DPC
    //

    if(SUCCEEDED(hr) && !(m_dwFlags & AC97_OBJECTF_DIRECTISR))
    {
        KeInitializeDpc(&m_dpc, AciInterruptDpcHandler, this);
    }

    //
    // Set up the interrupt handler
    //

    if(SUCCEEDED(hr))
    {

#ifdef AC97_AMR

        ulInterruptVector = HalGetInterruptVector(PciConfig.u.type0.InterruptLine, &Irql);

#else // AC97_AMR

        ulInterruptVector = HalGetInterruptVector(XPCICFG_ACI_IRQ, &Irql);

#endif // AC97_AMR

        KeInitializeInterrupt(&m_Interrupt, AciInterruptServiceRoutine, this, ulInterruptVector, Irql, LevelSensitive, TRUE);

        if(!KeConnectInterrupt(&m_Interrupt))
        {
            DPF_ERROR("Failed to connect interrupt");
            hr = DSERR_GENERIC;
        }
    }

    //
    // Unmute the CODEC
    //

    if(SUCCEEDED(hr))
    {
        if(!PokeAc97Register(AC97REG_FRONT_VOLUME, 0))
        {
            DPF_ERROR("Unable to set front volume");
            hr = DSERR_GENERIC;
        }
    }

    if(SUCCEEDED(hr))
    {
        if(!PokeAc97Register(AC97REG_PCM_OUT_VOLUME, 0x0808))
        {
            DPF_ERROR("Unable to set PCM out volume");
            hr = DSERR_GENERIC;
        }
    }

    //
    // Register for HAL shutdown notification
    //

    if(SUCCEEDED(hr))
    {
        m_HalShutdownData.NotificationRoutine = AciShutdownNotifier;

        HalRegisterShutdownNotification(&m_HalShutdownData, TRUE);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  Terminate
 *
 *  Description:
 *      Shuts down the driver object.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Device::Terminate"

void
CAc97Device::Terminate
(
    void
)
{
    DWORD                   i;

    DPF_ENTER();
    AutoIrql();

    //
    // Disconnect the interrupt handler
    //

    if(m_Interrupt.ServiceRoutine)
    {
        KeDisconnectInterrupt(&m_Interrupt);
        m_Interrupt.ServiceRoutine = NULL;
    }

    //
    // Clear any pending DPCs
    //

    if(DpcObject == m_dpc.Type)
    {
        KeRemoveQueueDpc(&m_dpc);
        m_dpc.Type = ~m_dpc.Type;
    }

    //
    // Unregister the shutdown handler
    //

    if(m_HalShutdownData.NotificationRoutine)
    {
        HalRegisterShutdownNotification(&m_HalShutdownData, FALSE);
        m_HalShutdownData.NotificationRoutine = NULL;
    }

    //
    // Free all channels
    //

    for(i = 0; i < AC97_CHANNELTYPE_COUNT; i++)
    {
        ReleaseChannel((AC97CHANNELTYPE)i);
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  CreateChannel
 *
 *  Description:
 *      Allocates a channel object.  The AC97 device object and it's channels
 *      are primitive enough that you can only allocate each channel once, and
 *      neither of them support ref counting.  Oh well...
 *
 *  Arguments:
 *      AC97CHANNELTYPE [in]: channel type.
 *      CAc97Channel ** [out]: receives channel object.  The caller can
 *                             release this object by calling ReleaseChannel,
 *                             but it's not required.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Device::CreateChannel"

HRESULT
CAc97Device::CreateChannel
(
    AC97CHANNELTYPE         nChannelType, 
    CAc97Channel **         ppChannel
)
{
    HRESULT                 hr;
    
    DPF_ENTER();
    AutoIrql();
    
    ASSERT(!m_apChannels[nChannelType]);

    hr = HRFROMP(m_apChannels[nChannelType] = NEW(CAc97Channel(this, nChannelType)));

    if(SUCCEEDED(hr) && ppChannel)
    {
        *ppChannel = m_apChannels[nChannelType];
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  ReleaseChannel
 *
 *  Description:
 *      Releases a previously allocated channel.
 *
 *  Arguments:
 *      AC97CHANNELTYPE [in]: channel type.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Device::ReleaseChannel"

void
CAc97Device::ReleaseChannel
(
    AC97CHANNELTYPE         Channel
)
{
    DPF_ENTER();
    AutoIrql();
    
    DELETE(m_apChannels[Channel]);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  AcquireCodecSemaphore
 *
 *  Description:
 *      Acquires access to the CODEC semaphore.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE if the semaphore was acquired.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Device::AcquireCodecSemaphore"

BOOL
CAc97Device::AcquireCodecSemaphore
(
    void
)
{
    DWORD                   dwAttempts  = 1000;
    BOOL                    fAcquired   = TRUE;

    DPF_ENTER();

    while(PeekAciRegister8(AC97_SEM4) & AC97_SEM4_OWNED)
    {
        if(!dwAttempts--)
        {
            DPF_ERROR("Failed to get CODEC semaphore");
            fAcquired = FALSE;
            break;
        }

        WaitRegisterRetry();
    }

    DPF_LEAVE(fAcquired);

    return fAcquired;
}


/****************************************************************************
 *
 *  CodecReady
 *
 *  Description:
 *      Checks the AC-Link state.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE if the CODEC is ready.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Device::CodecReady"

BOOL
CAc97Device::CodecReady
(
    void
)
{
    BOOL                    fReady      = TRUE;
    DWORD                   dwAttempts  = 1000;
    DWORD                   dwValue;
    
    DPF_ENTER();

    //
    // Get the value of the control register.
    //

    dwValue = PeekAciRegister32(GLB_CTRL);
    
    //
    // If the AC_RST# line is low, raise it and do a cold reset.
    //

    if(!(dwValue & GLB_CTRL_COLD))
    {
        DPF_AC97("Performing cold reset");
        
        dwValue |= GLB_CTRL_COLD;

        PokeAciRegister32(GLB_CTRL, dwValue);
    }

    //
    // If the AC link is down, raise it and do a warm reset.  This is only
    // needed if agressive power management is used.
    //

    if(dwValue & GLB_CTRL_ACSD)
    {
        DPF_AC97("Performing warm reset");
        
        dwValue &= ~(GLB_CTRL_ACSD | GLB_CTRL_WARM);

        PokeAciRegister32(GLB_CTRL, dwValue);
    }

    //
    // Check to see if the ready bit is set.  This takes a maximum of 200us.
    //

    while(!(PeekAciRegister32(GLB_STATUS) & GLB_STATUS_PCR))
    {
        if(!dwAttempts--)
        {
            DPF_ERROR("Timed out waiting for ready bit");
            fReady = FALSE;
            break;
        }

        WaitRegisterRetry();
    }

    if(fReady)
    {
        DPF_AC97("CODEC ready");
    }

    DPF_LEAVE(fReady);

    return fReady;
}


/****************************************************************************
 *
 *  GetInterruptStatus
 *
 *  Description:
 *      Determines if there is an interrupt pending for the device.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      DWORD: interrupt status.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Device::GetInterruptStatus"

DWORD
CAc97Device::GetInterruptStatus
(
    void
)
{
    static const DWORD      dwStatusMask    = GLB_STATUS_AOINT | GLB_STATUS_SOINT | GLB_STATUS_GPINT;

    return PeekAciRegister32(GLB_STATUS) & dwStatusMask;
}


/****************************************************************************
 *
 *  ServiceAciInterrupt
 *
 *  Description:
 *      Services interrupts for the device.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      DWORD: bitmask of the channels that triggered the interrupt.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Device::ServiceAciInterrupt"

DWORD
CAc97Device::ServiceAciInterrupt
(
    void
)
{
    static const DWORD      adwChannelStatusMasks[AC97_CHANNELTYPE_COUNT] = 
    { 
        GLB_STATUS_AOINT, 
        GLB_STATUS_SOINT,
    };

    DWORD                   dwInterruptMask;
    DWORD                   dwStatus;
    WORD                    wChannelStatus;
    DWORD                   i;

    DPF_ENTER();

    //
    // Get the ACI interrupt status bits
    //

    if(dwStatus = GetInterruptStatus())
    {
        //
        // Set the interrupt mask to a non-zero value that won't conflict
        // with any of the channel-specific values.
        //
        
        dwInterruptMask = 0x80000000;

        //
        // Handle channel interrupts
        //
        
        for(i = 0; i < AC97_CHANNELTYPE_COUNT; i++)
        {
            if(dwStatus & adwChannelStatusMasks[i])
            {
                if(m_apChannels[i])
                {
                    //
                    // Ok, this channel had an interrupt.  The only ones we
                    // really care about are buffer completion interrupts.
                    // If either of those are set in the channel status 
                    // registers, increment the channel's count of pending
                    // completions and schedule a DPC to handle them.
                    //

                    wChannelStatus = m_apChannels[i]->PeekAciRegister16(X_STATUS);

                    if(wChannelStatus & X_STATUS_FIFOE) 
                    {
                        DPF_AC97("(channel %lu) FIFO underrun", i);
                    }

                    if(wChannelStatus & X_STATUS_CIELV)
                    {
                        DPF_AC97("(channel %lu) Current index equals last valid", i);
                    }

                    if(wChannelStatus & X_STATUS_LVBCI)
                    {
                        DPF_AC97("(channel %lu) Last valid buffer completion interrupt", i);
                    }
                
                    if(wChannelStatus & X_STATUS_BCI)
                    {
                        DPF_AC97("(channel %lu) Buffer completed", i);

                        dwInterruptMask |= AC97_CHANNELINTERRUPT(i);

                        if(m_dwFlags & AC97_OBJECTF_DIRECTISR)
                        {
                            m_apChannels[i]->ServiceInterrupt();
                        }
                        else
                        {
                            m_abPendingBufferCompletions[i][0]++;
                        }
                    }

                    m_apChannels[i]->PokeAciRegister16(X_STATUS, wChannelStatus);
                }
                else
                {
                    DPF_WARNING("Channel %lu not allocated!", i);
                }
            }
        }
    
        //
        // Schedule a DPC to handle any buffer completion interrupts
        //

        if(!(m_dwFlags & AC97_OBJECTF_DIRECTISR) && (dwInterruptMask & AC97_CHANNELINTERRUPT_MASK))
        {
            KeInsertQueueDpc(&m_dpc, NULL, NULL);
        }

        //
        // Acknowlege the interrupt
        //
        
        PokeAciRegister32(GLB_STATUS, dwStatus & ~GLB_STATUS_GPINT);
    }
    else
    {
        //
        // Not our interrupt
        //

        dwInterruptMask = 0;
    }

    DPF_LEAVE(dwInterruptMask);

    return dwInterruptMask;
}


/****************************************************************************
 *
 *  ServiceAciInterruptDpc
 *
 *  Description:
 *      Services interrupts for the device.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Device::ServiceAciInterruptDpc"

void
CAc97Device::ServiceAciInterruptDpc
(
    void
)
{
    BOOL                    fContinue;
    DWORD                   i;

    DPF_ENTER();

    do
    {
        //
        // Copy live buffer completion counts to cached
        //

        KeSynchronizeExecution(&m_Interrupt, AciSynchronizationRoutine, this);

        //
        // Handle all pending buffer completions
        //

        for(i = 0, fContinue = FALSE; i < AC97_CHANNELTYPE_COUNT; i++)
        {
            if(m_abPendingBufferCompletions[i][1])
            {
                fContinue = TRUE;

                if(m_apChannels[i])
                {
                    do
                    {
                        m_apChannels[i]->ServiceInterrupt();
                    }
                    while(--m_abPendingBufferCompletions[i][1]);
                }
                else
                {
                    m_abPendingBufferCompletions[i][1] = 0;
                }
            }
        }
    }
    while(fContinue);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SynchronizeAciInterrupt
 *
 *  Description:
 *      Interrupt synchonization function.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Device::SynchronizeAciInterrupt"

void
CAc97Device::SynchronizeAciInterrupt
(
    void
)
{
    DWORD                   i;

    DPF_ENTER();

    //
    // Copy live pending buffer completions to cached and clear the live
    // ones.
    //

    for(i = 0; i < AC97_CHANNELTYPE_COUNT; i++)
    {
        m_abPendingBufferCompletions[i][1] += m_abPendingBufferCompletions[i][0];

        m_abPendingBufferCompletions[i][0] = 0;
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  PeekAc97Register
 *
 *  Description:
 *      Gets the contents of an AC '97 register.
 *
 *  Arguments:
 *      AC97REGISTER [in]: register.
 *      LPWORD [out]: register value.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Device::PeekAc97Register"

BOOL
CAc97Device::PeekAc97Register
(
    AC97REGISTER            reg,
    LPWORD                  pwValue
)
{
    DWORD                   dwStatus;
    BOOL                    fSuccess;

    DPF_ENTER();
    
    //
    // Acquire the CODEC semaphore
    //

    fSuccess = AcquireCodecSemaphore();

    //
    // Read the contents of the register
    //

    if(fSuccess)
    {
        *pwValue = PeekRegister16(m_dwAc97RegisterBase + (reg * 2));
    }

    //
    // Make sure the read was successful
    //

    if(fSuccess)
    {
        if((dwStatus = PeekAciRegister32(GLB_STATUS)) & GLB_STATUS_RCS)
        {
            //
            // Nope.  Clear the bit.
            //

            DPF_ERROR("AC '97 register peek failed");
            
            PokeAciRegister32(GLB_STATUS, dwStatus & ~GLB_STATUS_RCS);
            
            fSuccess = FALSE;
        }
    }

    DPF_LEAVE(fSuccess);

    return fSuccess;
}


/****************************************************************************
 *
 *  PokeAc97Register
 *
 *  Description:
 *      Writes the contents of an AC '97 register.
 *
 *  Arguments:
 *      AC97REGISTER [in]: register.
 *      WORD [in]: register value.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Device::PokeAc97Register"

BOOL
CAc97Device::PokeAc97Register
(
    AC97REGISTER            reg,
    WORD                    wValue
)
{
    BOOL                    fSuccess;

    DPF_ENTER();
    
    //
    // Acquire the CODEC semaphore
    //

    fSuccess = AcquireCodecSemaphore();

    //
    // Write to the register
    //

    if(fSuccess)
    {
        PokeRegister16(m_dwAc97RegisterBase + (reg * 2), wValue);
    }

    DPF_LEAVE(fSuccess);

    return fSuccess;
}


/****************************************************************************
 *
 *  VerifyPokeAc97Register
 *
 *  Description:
 *      Writes the contents of an AC '97 register and verifies that it 
 *      stuck.
 *
 *  Arguments:
 *      AC97REGISTER [in]: register.
 *      WORD [in]: register value.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Device::VerifyPokeAc97Register"

BOOL
CAc97Device::VerifyPokeAc97Register
(
    AC97REGISTER            reg,
    WORD                    wValue
)
{
    WORD                    wNewValue;
    BOOL                    fSuccess;

    DPF_ENTER();
    
    //
    // Write the new value
    //

    fSuccess = PokeAc97Register(reg, wValue);

    //
    // Read back the new value
    //

    if(fSuccess)
    {
        fSuccess = PeekAc97Register(reg, &wNewValue);
    }

    //
    // Did it stick?
    //

    if(fSuccess && (wValue != wNewValue))
    {
        fSuccess = FALSE;
    }

    DPF_LEAVE(fSuccess);

    return fSuccess;
}


/****************************************************************************
 *
 *  PowerUp
 *
 *  Description:
 *      Sets the CODEC to full power.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Device::PowerUp"

HRESULT
CAc97Device::PowerUp
(
    void
)
{
    HRESULT                 hr  = DS_OK;
    
    DPF_ENTER();

    //
    // Set the CODEC to full power
    //

    if(!PokeAc97Register(AC97REG_POWERDOWN, AC97REG_PCS_FULL_POWER))
    {
        DPF_ERROR("Failed to set power state");
        hr = DSERR_GENERIC;
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  CAc97Channel
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      CAc97Device * [in]: parent device.
 *      DWORD [in]: channel register base offset.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Channel::CAc97Channel"

const DWORD CAc97Channel::m_adwRegisterOffsets[AC97_CHANNELTYPE_COUNT] = 
{ 
    AO_BL_BASE, 
    SO_BL_BASE,
};

const DWORD CAc97Channel::m_dwSilenceSize = 0x2000;

CAc97Channel::CAc97Channel
(
    CAc97Device *           pDevice,
    AC97CHANNELTYPE         nChannelType
)
:   m_nChannelType(nChannelType)
{

    DPF_ENTER();

    //
    // Initialize defaults
    //

    m_dwMode = DSAC97_MODE_DEFAULT;

    //
    // Save a pointer back to the parent device.  Note that we're not 
    // AddRef'ing it, since the device controls our construction and 
    // destruction.
    //

    m_pDevice = pDevice;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CAc97Channel
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
#define DPF_FNAME "CAc97Channel::~CAc97Channel"

CAc97Channel::~CAc97Channel
(
    void
)
{
    DPF_ENTER();
    
    Terminate();

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
 *      DWORD [in]: flags.
 *      LPFNAC97CHANNELCALLBACK [in]: interrupt callback function.
 *      LPVOID [in]: interrupt callback context.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Channel::Initialize"

HRESULT
CAc97Channel::Initialize
(
    DWORD                   dwFlags,
    LPFNAC97CHANNELCALLBACK pfnCallback,
    LPVOID                  pvContext
)
{
    HRESULT                 hr;
    
    DPF_ENTER();
    AutoIrql();


    m_dwFlags = dwFlags;
    m_pfnCallback = pfnCallback;
    m_pvCallbackContext = pvContext;

    //
    // Allocate the PRDL
    //

    hr = HRFROMP(m_pPrdl = PHYSALLOC(ACIPRD, ACIPRDL_ENTRY_COUNT, PAGE_SIZE, PAGE_READWRITE));

    if(SUCCEEDED(hr))
    {
        m_dwPrdlAddress = MmGetPhysicalAddress(m_pPrdl);
    }

    //
    // If we're a stream, allocate a packet of silence to loop on when we
    // starve.
    //

    if(SUCCEEDED(hr) && (dwFlags & AC97CHANNEL_OBJECTF_STREAM))
    {
        hr = HRFROMP(m_pvSilence = PHYSALLOC(BYTE, m_dwSilenceSize, PAGE_SIZE, PAGE_READWRITE));
    }

    //
    // Reset the channel
    //

    if(SUCCEEDED(hr))
    {
        Reset();
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  Terminate
 *
 *  Description:
 *      Uninitializes the object.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Channel::Terminate"

void
CAc97Channel::Terminate
(
    void
)
{
    DPF_ENTER();
    AutoIrql();

    //
    // Reset the channel
    //

    Flush();
    
    //
    // Free resources
    //

    PHYSFREE(m_pPrdl);
    PHYSFREE(m_pvSilence);

    m_dwPrdlAddress = 0;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  AttachPacket
 *
 *  Description:
 *      Attaches an audio data packet to be played.
 *
 *  Arguments:
 *      DWORD [in]: data buffer physical memory address.
 *      DWORD [in]: data buffer size, in bytes.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Channel::AttachPacket"

void
CAc97Channel::AttachPacket
(
    DWORD                   dwBufferAddress,
    DWORD                   dwBufferSize
)
{
    BYTE                    bPrdIndex;
    
    DPF_ENTER();
    AutoIrql();

    ASSERT(m_bPrdCount < ACIPRDL_ENTRY_COUNT);
    ASSERT((dwBufferSize / (m_pDevice->m_wfxFormat.wBitsPerSample / 8)) <= 0xFFFF);
    ASSERT(!(dwBufferSize % m_pDevice->m_wfxFormat.nBlockAlign));

    if(!(m_dwFlags & AC97CHANNEL_OBJECTF_STREAM))
    {
        ASSERT(!m_bCurrentPrd);
    }

    //
    // If a discontinuity has been signaled, but another packet has been
    // attached, we can remove the discontinuity by resetting the flags and
    // removing the silence packet.  The easiest way to remove the silence
    // packet is to just stomp on the PRD.
    //

    if(m_dwFlags & AC97CHANNEL_OBJECTF_DISCONTINUITY)
    {
        ASSERT(m_dwFlags & AC97CHANNEL_OBJECTF_STREAM);
        ASSERT(m_bPrdCount);
        
        m_dwFlags &= ~AC97CHANNEL_OBJECTF_DISCONTINUITY;

        m_bPrdCount--;
    }

    //
    // Add the PRD to the end of the list.  We're assuming that the buffer
    // is already locked.
    //

    bPrdIndex = (m_bCurrentPrd + m_bPrdCount) & ACIPRDL_ENTRY_MASK;

    m_pPrdl[bPrdIndex].dwPhysicalAddress = dwBufferAddress;
    m_pPrdl[bPrdIndex].wLength = LOWORD(dwBufferSize / (m_pDevice->m_wfxFormat.wBitsPerSample / 8));
    m_pPrdl[bPrdIndex].wReserved = 0;
    m_pPrdl[bPrdIndex].fBufferUnderrunPolicy = TRUE;
    m_pPrdl[bPrdIndex].fInterruptOnCompletion = TRUE;

    DPF_AC97("(channel %lu) PRD %d set to %x %x", m_nChannelType, (int)bPrdIndex, m_pPrdl[bPrdIndex].dwPhysicalAddress, (DWORD)m_pPrdl[bPrdIndex].wLength);

    //
    // Inform the hardware that the last valid PRD has changed
    //

    SetPrdIndeces(m_bCurrentPrd, m_bPrdCount + 1, FALSE, TRUE);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  AttachBuffer
 *
 *  Description:
 *      Attaches an audio data packet to be played.
 *
 *  Arguments:
 *      DWORD [in]: data buffer physical memory address.
 *      DWORD [in]: data buffer size, in bytes.
 *      DWORD [in]: count of packets to split the buffer into.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Channel::AttachBuffer"

void
CAc97Channel::AttachBuffer
(
    DWORD                   dwBufferAddress,
    DWORD                   dwBufferSize,
    DWORD                   dwPacketCount
)
{
    DPF_ENTER();
    AutoIrql();

    //
    // Attach the buffer in as many packets as were specified
    //

    ASSERT(!(dwBufferSize % dwPacketCount));

    dwBufferSize /= dwPacketCount;

    while(dwPacketCount--)
    {
        AttachPacket(dwBufferAddress, dwBufferSize);

        dwBufferAddress += dwBufferSize;
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Discontinuity
 *
 *  Description:
 *      Signals that the stream may starve.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Channel::Discontinuity"

void
CAc97Channel::Discontinuity
(
    void
)
{
    DPF_ENTER();
    AutoIrql();

    ASSERT(m_dwFlags & AC97CHANNEL_OBJECTF_STREAM);

    //
    // Attach a packet of silence that the DMA can spin on when we starve.
    //

    if(!(m_dwFlags & AC97CHANNEL_OBJECTF_DISCONTINUITY))
    {
        AttachPacket(m_pvSilence, m_dwSilenceSize);

        m_dwFlags |= AC97CHANNEL_OBJECTF_DISCONTINUITY;
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ServiceInterrupt
 *
 *  Description:
 *      Services interrupts for the channel.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Channel::ServiceInterrupt"

void
CAc97Channel::ServiceInterrupt
(
    void
)
{
    DPF_ENTER();

    if(m_bPrdCount)
    {
        //
        // A buffer has completed.  If we're a stream, we'll need to increment
        // the first PRD index and decrement the count in order to detach the
        // packet.
        //
        // If we're out of PRDs, stop the DMA.  This only happens because you
        // starved the stream.  Until we get a chance to stop the DMA, it's 
        // going to continue to play the last PRD over again.  If you know 
        // you're going to starve me, attach a packet of silence.
        //

        m_dwPosition += m_pPrdl[m_bCurrentPrd].wLength;

        if(m_dwFlags & AC97CHANNEL_OBJECTF_STREAM)
        {
            if(m_bPrdCount > 2)
            {
                SetPrdIndeces(m_bCurrentPrd + 1, m_bPrdCount - 1, FALSE, FALSE);
            }
            else
            {
                if(!(m_dwFlags & AC97CHANNEL_OBJECTF_DISCONTINUITY))
                {
                    DPF_WARNING("AC'97 stream starving");
                }
                
                Flush(FALSE);
            }
        }
        else
        {
            SetPrdIndeces(m_bCurrentPrd + 1, m_bPrdCount, FALSE, FALSE);
        }

        //
        // Notify the callback function
        //

        if(m_pfnCallback)
        {
            m_pfnCallback(m_pvCallbackContext);
        }
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Reset
 *
 *  Description:
 *      Resets DMA registers.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Channel::Reset"

void
CAc97Channel::Reset
(
    void
)
{
    DPF_ENTER();
    AutoIrql();

    DPF_AC97("(channel %lu) Resetting DMA registers", m_nChannelType);

    //
    // Reset the channel
    //

    PokeAciRegister8(X_CTRL, X_CTRL_RBMR);

    //
    // Wait for the reset to be acknowledged
    //

    while(PeekAciRegister8(X_CTRL) & X_CTRL_RBMR);

    //
    // Set the PRDL location
    //

    PokeAciRegister32(X_BL_BASE, m_dwPrdlAddress);

    //
    // If this is the S/PDIF channel, set the output mode
    //

    if(AC97_CHANNELTYPE_DIGITAL == m_nChannelType)
    {
        PokeAciRegister32(X_STWD, m_dwMode);
    }

    //
    // Reset PRD indeces
    //

    SetPrdIndeces(m_bCurrentPrd, m_bPrdCount, TRUE, TRUE);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Flush
 *
 *  Description:
 *      Resets the object and flushes all packets.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to reset the cursor position.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Channel::Flush"

void
CAc97Channel::Flush
(
    BOOL                    fResetPosition
)
{
    BYTE                    bState;

    DPF_ENTER();
    AutoIrql();

    //
    // Stop the DMA
    //
    
    Pause();

    //
    // Reset the channel
    //

    Reset();

    //
    // Update PRD indeces
    //

    SetPrdIndeces(0, 0, TRUE, TRUE);

    //
    // Remove any existing discontinuity
    //

    m_dwFlags &= ~AC97CHANNEL_OBJECTF_DISCONTINUITY;

    //
    // Reset stored positions
    //

    if(fResetPosition)
    {
        m_dwPosition = 0;
        m_dwLastPosition = 0;
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Run
 *
 *  Description:
 *      Starts the DMA running.
 *
 *  Arguments:
 *      DWORD [in]: PRD position.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Channel::Run"

void
CAc97Channel::Run
(
    DWORD                   dwPosition
)
{
    DPF_ENTER();
    AutoIrql();

    ASSERT(m_bPrdCount);

    //
    // Check the channel status.  If the DMA is halted, we'll have to
    // reset before we can run.
    //

    if(PeekAciRegister16(X_STATUS) & X_STATUS_DCH)
    {
        Reset();
    }

    //
    // Set the buffer position
    //

    if(-1 != dwPosition)
    {
        SetPosition(dwPosition);
    }

    //
    // Set the channel to RUN state and enable interrupts
    //

    PokeAciRegister8(X_CTRL, X_CTRL_IOCE | X_CTRL_FEIE | X_CTRL_LVBIE | X_CTRL_RPBM);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Pause
 *
 *  Description:
 *      Pauses the DMA.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Channel::Pause"

void
CAc97Channel::Pause
(
    void
)
{
    DPF_ENTER();
    AutoIrql();

    PokeAciRegister8(X_CTRL, 0);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetPrdIndeces
 *
 *  Description:
 *      Sets the current PRDL index and count.
 *
 *  Arguments:
 *      DWORD [in]: index.
 *      DWORD [in]: count.
 *      BOOL [in]: TRUE to apply the current PRD index to the hardware.
 *                 In some cases, we may be calling this function late and it
 *                 may be better not to overwrite the hardware value.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Channel::SetPrdIndeces"

void
CAc97Channel::SetPrdIndeces
(
    BYTE                    bCurrentPrd,
    BYTE                    bPrdCount,
    BOOL                    fApplyCurrent,
    BOOL                    fApplyLast
)
{
    BYTE                    bLastPrd;
    
    DPF_ENTER();
    AutoIrql();

    //
    // Calculate the current and last valid PRDs
    //

    if(bPrdCount)
    {
        ASSERT(bPrdCount <= ACIPRDL_ENTRY_COUNT);

        bLastPrd = bCurrentPrd + bPrdCount - 1;

        if(m_dwFlags & AC97CHANNEL_OBJECTF_STREAM)
        {
            bCurrentPrd &= ACIPRDL_ENTRY_MASK;
            bLastPrd &= ACIPRDL_ENTRY_MASK;
        }
        else
        {
            bCurrentPrd %= bPrdCount;
            bLastPrd %= bPrdCount;
        }
    }
    else
    {
        bCurrentPrd = 0;
        bLastPrd = 0;
    }

    //
    // Inform the hardware
    //
    
    if(fApplyCurrent)
    {
        PokeAciRegister8(X_CUR_IDX, bCurrentPrd);
    }

    if(fApplyLast)
    {
        PokeAciRegister8(X_LST_IDX, bLastPrd);
    }

    m_bCurrentPrd = bCurrentPrd;
    m_bPrdCount = bPrdCount;

    DPF_AC97("(channel %lu) PRD indeces set to %d, %d (%d)", m_nChannelType, (int)bCurrentPrd, (int)bLastPrd, (int)bPrdCount);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetMode
 *
 *  Description:
 *      Sets the digital channel's output mode.
 *
 *  Arguments:
 *      DWORD [in]: mode.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Channel::SetMode"

void
CAc97Channel::SetMode
(
    DWORD                   dwMode
)
{
    BYTE                    bState;
    
    DPF_ENTER();
    AutoIrql();

    if(AC97_CHANNELTYPE_DIGITAL == m_nChannelType)
    {
        //
        // Save the mode
        //

        m_dwMode = dwMode;

        //
        // Update the STWD register
        //

        PokeAciRegister32(X_STWD, m_dwMode);
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  GetPosition
 *
 *  Description:
 *      Gets the buffer or stream position
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      DWORD: absolute stream position.
 *
 ****************************************************************************/

#ifndef MCPX_BOOT_LIB

#undef DPF_FNAME
#define DPF_FNAME "CAc97Channel::GetPosition"

DWORD
CAc97Channel::GetPosition
(
    void
)
{
    DWORD               dwLastPosition;
    DWORD               dwPosition;
    DWORD               dwLength;
    BYTE                bCurrentPrd;
        
    DPF_ENTER();

    if(m_bPrdCount)
    {
        //
        // Calculate the position until we get the same position twice.
        // That way, we don't have to synchronize this function with the
        // ISR.
        //
    
        dwPosition = -1;

        int iTry = 0;

        while(TRUE)
        {
            iTry++;
            
            //
            // Save the last position we got
            //

            dwLastPosition = dwPosition;

            //
            // Check the PRD index register and compare it to our own index.
            // There seems to be some delay between when the DMA program
            // changes PRDs and when we get notified.
            //

            if((bCurrentPrd = PeekAciRegister8(X_CUR_IDX)) == m_bCurrentPrd)
            {

#ifdef DEBUG
                //
                // Since they're in sync, we should never be playing the
                // discontinuity packet.
                //

                if(m_dwFlags & AC97CHANNEL_OBJECTF_DISCONTINUITY)
                {
                    if(m_bPrdCount <= 1)
                    {
                        if((bCurrentPrd = PeekAciRegister8(X_CUR_IDX)) == m_bCurrentPrd)
                        {
                            ASSERTMSG("Improperly looping on the discontinuity packet");
                        }
                    }
                }

#endif // DEBUG

                //
                // The position in the ACI register is really the count of samples 
                // left to transfer.
                //

                dwLength = m_pPrdl[m_bCurrentPrd].wLength;

                dwPosition = PeekAciRegister16(X_CUR_POS);

                if(dwPosition <= dwLength)
                {
                    dwPosition = dwLength - dwPosition;
                }
                else
                {
                    dwPosition = 0;
                }
            }
            else
            {
                //
                // Our internal state and the hardware's aren't in synch.  Simply
                // return that we're at the end of the current packet
                //

                dwPosition = m_pPrdl[m_bCurrentPrd].wLength;
            }

            //
            // Add the absolute stream position.  If you're a buffer, well, you'll
            // need to MOD this value with the buffer size.  We don't currently 
            // have any buffer clients of this function, so why bother writing
            // the code?
            //

            dwPosition += m_dwPosition;

            //
            // Compare this position to the last one we calculated.  If they 
            // match, we can quit looping.
            //

            if(dwLastPosition == dwPosition)
            {
                //
                // Compare this position to the last one we returned.  If the 
                // current position is less than the last one, the buffer has 
                // wrapped, but the interrupt hasn't happened yet.
                //

                if(dwPosition >= m_dwLastPosition)
                {
                    m_dwLastPosition = dwPosition;
                }
                else
                {
                    dwPosition = m_dwLastPosition;
                }

                break;
            }

            //
            // If we're at raised IRQL, we can't loop
            //

            if(KeGetCurrentIrql() >= DISPATCH_LEVEL)
            {
                break;
            }
        }
    }
    else
    {
        dwPosition = m_dwPosition;
    }

    //
    // Convert sample position to bytes
    //

    dwPosition *= m_pDevice->m_wfxFormat.wBitsPerSample / 8;

    DPF_LEAVE(dwPosition);

    return dwPosition;
}

#endif


/****************************************************************************
 *
 *  SetPosition
 *
 *  Description:
 *      Sets the current PRD-relative position.
 *
 *  Arguments:
 *      DWORD [in]: PRD position.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97Channel::SetPosition"

void
CAc97Channel::SetPosition
(
    DWORD               dwPosition
)
{
    DWORD               dwLength;

    DPF_ENTER();

    //
    // Make sure we're stopped
    //

    ASSERT(!PeekAciRegister8(X_CTRL));

    //
    // Convert byte position to samples
    //

    dwPosition /= m_pDevice->m_wfxFormat.wBitsPerSample / 8;

    //
    // Convert buffer position to samples remaining
    //

    dwLength = m_pPrdl[m_bCurrentPrd].wLength;
    ASSERT(dwPosition <= dwLength);
    
    dwPosition = dwLength - dwPosition;

    //
    // Set the position
    //

    ASSERT(dwPosition < 0xFFFF);
    PokeAciRegister16(X_CUR_POS, (WORD)dwPosition);

    //
    // Clear the last returned position
    //

    m_dwLastPosition = 0;

    DPF_LEAVE_VOID();
}


