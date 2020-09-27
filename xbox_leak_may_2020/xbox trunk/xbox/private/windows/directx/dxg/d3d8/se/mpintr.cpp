/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mpintr.cpp
 *  Content:    interrupt service related functions
 *
 ***************************************************************************/

#include "precomp.hpp"

#include <ntos.h>
#include <pci.h>

//
// Note that we optimize this module for speed, not for size, since it
// is responsible for our interrupt code
//

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{
#include "mp.hpp"

#if DBG
bool g_DpcRIPFired;
#define DPCRIP(x) { DXGRIP(x); g_DpcRIPFired = true; }

VOID
ReportHardwareError(
    CONST CHAR* Message
    );

#else
#define DPCRIP(x) DXGRIP(x)
#endif


extern "C" void __cdecl _disable(void);
extern "C" void __cdecl _enable(void);

#pragma intrinsic(_disable)
#pragma intrinsic(_enable)



DWORD g_VideoOffset;

BOOLEAN
CMiniport::Isr(
    IN PKINTERRUPT InterruptObject,
    IN PVOID ServiceContext
    )

{
    CMiniport* miniport = (CMiniport*) ServiceContext;
    BYTE* regbase = (BYTE*)(miniport->m_RegisterBase);
    ULONG intr = 0;
    BOOL is1080i;

    //
    // We're only interested in the interrupt only if:
    //  - we are initialized and prepared to handle interrupts
    //  - interrupts are enabled
    //  - there are pending interrupt requests
    //
    // NOTE: It's very expensive to read any hardware registers, including
    //       NV_PMC_INTR_EN_0 and NV_PMC_INTR_0, so we do everything we can 
    //       here to avoid reading them (since we have to do it in the DPC
    //       anyways).
    //
    if (miniport->m_InterruptsEnabled && REG_RD32(regbase, NV_PMC_INTR_EN_0))
    {
        // Burning a local to keep the next if from getting too unweildy.
        is1080i = (miniport->m_DisplayMode & AV_MODE_OUT_MASK) == AV_MODE_OUT_HDTV
                   && (miniport->m_CurrentAvInfo & AV_FLAGS_INTERLACED);

        // We need very accurate sampling of the current field when running
        // in field-rendered mode.  
        //
        if (!miniport->m_UnhandledVBI 
            && ((miniport->m_CurrentAvInfo & AV_FLAGS_FIELD)
               || is1080i))
        {
            intr = REG_RD32(regbase, NV_PMC_INTR_0);
        
            if (intr & DRF_DEF(_PMC, _INTR_0, _PCRTC, _PENDING))
            {
                // The field that we're about to display is the opposite of the one we have.
                miniport->m_IsOddField = !(_inp(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + 0xC0) & 0x20);

                // The value of this pin in PAL is backwards.
                if ((miniport->m_DisplayMode & AV_MODE_OUT_MASK) == AV_MODE_OUT_525SDTV)
                { 
                    miniport->m_IsOddField = !miniport->m_IsOddField;
                }

                miniport->m_UnhandledVBI = TRUE;

                //
                // 1080i scans so fast that handling the flip in the DPC can
                // result in tearing.  So now we set where to scan from...everything
                // else is processed in the DPC.
                //

                //
                // NOTE: This logic must match that in VBlank!
                //
                // We add 1 to m_VBlankCount in this calculation because
                // unlike VBlank, we don't increment m_VBlankCount here.
                //
                BOOL expectFlip
                    = (miniport->m_VBlankCount + 1 - miniport->m_VBlankCountAtLastFlip) 
                            >= miniport->m_VBlanksBetweenFlips;

                DWORD flipIndex = miniport->m_VBlankFlipCount & (MAX_QUEUED_FLIPS - 1);

                BOOL doFlip
                    = expectFlip && miniport->m_VBlankFlips[flipIndex].Pending;

                if (doFlip)
                {
                    g_VideoOffset = miniport->m_VBlankFlips[flipIndex].Offset;
                }

                //
                // 1080i requires that we reprogram the video offset for each 
                // field.
                //
                miniport->DacProgramVideoStart(g_VideoOffset);
            }
        }

        //
        // Disable further interrupts
        // and dispatch our interrupt DPC routine
        //
        miniport->DisableInterrupts(regbase);

        KeInsertQueueDpc(&miniport->m_Dpc, NULL, NULL);
        return TRUE;
    }

    return FALSE;
}


VOID
CMiniport::Dpc(
    IN PKDPC Dpc,
    IN PVOID DpcContext,
    IN PVOID Arg1,
    IN PVOID Arg2
    )

{
    CMiniport* miniport = (CMiniport*) DpcContext;
    BYTE* regbase = (BYTE*)(miniport->m_RegisterBase);
    ULONG pending;

    // SRC: mcService()

    do 
    {
        pending = 0;
        ULONG intr = REG_RD32(regbase, NV_PMC_INTR_0);

        //
        // Service graphics engine interrupts
        //
        if (intr & DRF_DEF(_PMC, _INTR_0, _PGRAPH, _PENDING)) 
        {
            pending |= miniport->ServiceGrInterrupt();
        }                    
        
        ASSERT(REG_RD_DRF(regbase, _PFIFO, _DEBUG_0, _CACHE_ERROR0)
                != NV_PFIFO_DEBUG_0_CACHE_ERROR0_PENDING);

        if (intr & ~DRF_DEF(_PMC, _INTR_0, _PGRAPH, _PENDING))
        {
            //
            // Service timer interrupts (normally, these are turned off)
            //
            if (intr & DRF_DEF(_PMC, _INTR_0, _PTIMER, _PENDING)) 
            {
                REG_WR_DRF_DEF(regbase, _PTIMER, _INTR_0, _ALARM, _RESET);
                pending |= REG_RD32(regbase, NV_PTIMER_INTR_0);
            }
    
            //
            // Service VBlank interrupt
            //
            if (intr & (DRF_DEF(_PMC, _INTR_0, _PCRTC, _PENDING)))
            {
                COUNT_PERFEVENT(D3DPERFEvent_VBlank, FALSE);

                pending |= miniport->VBlank();
            }
    
            //
            // Service media port interrupts
            //
            if (intr & DRF_DEF(_PMC, _INTR_0, _PMEDIA, _PENDING)) 
            {
                pending |= miniport->ServiceMediaPortInterrupt();
            }
    
            //
            // Service FIFO interrupts
            //
            if (intr & DRF_DEF(_PMC, _INTR_0, _PFIFO, _PENDING))
            {
                pending |= miniport->ServiceFifoInterrupt();
            }
    
            //
            // Service video interrupts
            //
            if (intr & DRF_DEF(_PMC, _INTR_0, _PVIDEO, _PENDING)) 
            {
                pending |= miniport->ServiceVideoInterrupt();
            }
        }

    } while (pending);

    miniport->m_UnhandledVBI = FALSE;

    miniport->EnableInterrupts(regbase);
}

BOOL
CMiniport::IsFlipPending()
{
    DWORD flipIndex = m_VBlankFlipCount & (MAX_QUEUED_FLIPS - 1);

    return m_VBlankFlips[flipIndex].Pending;
}

VOID
CMiniport::VBlankFlip(
    ULONG Offset,
    ULONG FlipTime
    )
{
    BYTE* regbase = (BYTE*)m_RegisterBase;

    // Program the hardware.
    DacProgramVideoStart(Offset);

    //
    // Process gamma ramps.  We choose between the two gamma ramps based 
    // on the LSB of the current flip count
    //
    ULONG RampNo = (m_VBlankFlipCount & 1);
    if (m_GammaUpdated[RampNo] == TRUE)
    {
        DacProgramGammaRamp(&(m_GammaRamp[RampNo]));
        m_GammaUpdated[RampNo] = FALSE;
    }

    // Increment the read value

    FLD_WR_DRF_DEF(regbase, _PGRAPH, _INCREMENT, _READ_3D, _TRIGGER);

    m_VBlankFlipCount++;
    m_VBlankCountAtLastFlip = m_VBlankCount;
    m_TimeOfLastFlip = FlipTime;
}


ULONG
CMiniport::VBlank()
{
    BYTE* regbase = (BYTE*)m_RegisterBase;
    ULONG intr;

    // Calculate the time interval between VBlanks
    DWORD currentTime = GetTime();
    if (m_TimeOfLastVBlank != 0)
    {
        m_TimeBetweenVBlanks = currentTime - m_TimeOfLastVBlank;
    }
    m_TimeOfLastVBlank = currentTime;

    // Save the CRTC index reg
    BYTE crtcIndex = REG_RD08(regbase, NV_PRMCIO_CRX__COLOR);

    // Increment VBlank counter
    m_VBlankCount++;

    //
    // If D3DPRESENT_INTERVAL_TWO was set, let the flip happen only
    // if this is at least the second VBlank since the last flip
    //
    // NOTE: This logic must match that in Isr!
    //
    BOOL expectFlip 
        = (m_VBlankCount - m_VBlankCountAtLastFlip) >= m_VBlanksBetweenFlips;

    DWORD flipIndex = m_VBlankFlipCount & (MAX_QUEUED_FLIPS - 1);

    BOOL doFlip 
        = expectFlip && m_VBlankFlips[flipIndex].Pending;

    //
    // Process a flip
    //
    if (doFlip)
    {
        VBlankFlip(m_VBlankFlips[flipIndex].Offset, currentTime);

        m_VBlankFlips[flipIndex].Pending = FALSE;
    }

    //
    // Record the current field if we haven't done so already.
    //

    if (!(m_CurrentAvInfo & AV_FLAGS_FIELD))
    {
        // The field we're about to display is the opposite of the one we have.
        m_IsOddField = !(_inp(XPCICFG_LPCBRIDGE_IO_REGISTER_BASE_0 + 0xC0) & 0x20);

        // The value of this pin in PAL is backwards.
        if ((m_DisplayMode & AV_MODE_OUT_MASK) == AV_MODE_OUT_525SDTV)
        { 
            m_IsOddField = !m_IsOddField;
        }
    }

    // 
    // Clear interrupt bits
    //
    do 
    {
        DAC_REG_WR_DRF_DEF(regbase, _PCRTC, _INTR_0, _VBLANK, _RESET);
        intr = REG_RD32(regbase, NV_PMC_INTR_0);
    } while (intr & DRF_DEF(_PMC, _INTR_0, _PCRTC, _PENDING));

    // Set the vblank event.
    KeSetEvent(&m_VerticalBlankEvent, 1, FALSE);

    // Do the callback.
    if (m_pVerticalBlankCallback)
    {
        D3DVBLANKDATA data;

        data.VBlank = m_VBlankCount;
        data.Swap = m_VBlankFlipCount;
        data.Flags = 0;
        if (doFlip)
            data.Flags = D3DVBLANK_SWAPDONE;
        else if ((expectFlip) && (m_VBlanksBetweenFlips != 0))
            data.Flags = D3DVBLANK_SWAPMISSED;

        m_pVerticalBlankCallback(&data);
    }

    // Restore crtc index
    REG_WR08(regbase, NV_PRMCIO_CRX__COLOR, crtcIndex);
    
    return 0;
}


ULONG
CMiniport::ServiceGrInterrupt()

{
    BYTE* regbase = (BYTE*)m_RegisterBase;
    ULONG intr;


    // Disable FIFO from writing to the graphics engine

    REG_WR_DRF_DEF(regbase, _PGRAPH, _FIFO, _ACCESS, _DISABLED);
    intr = REG_RD32(regbase, NV_PGRAPH_INTR);

    // Read graphics engine exception data

    ULONG Offset = REG_RD32(regbase, NV_PGRAPH_TRAPPED_ADDR);
    ULONG NotifySource = REG_RD32(regbase, NV_PGRAPH_NSOURCE);

    Offset &= (DRF_MASK(NV_PGRAPH_TRAPPED_ADDR_MTHD) <<
               DRF_SHIFT(NV_PGRAPH_TRAPPED_ADDR_MTHD));

    // Exception handling code

    if (intr & DRF_DEF(_PGRAPH, _INTR, _CONTEXT_SWITCH, _PENDING)) 
    {
        ULONG ChID = DRF_VAL(_PGRAPH, _TRAPPED_ADDR, _CHID, Offset);

        // Reset condition.
        
        REG_WR_DRF_DEF(regbase, _PGRAPH, _INTR, _CONTEXT_SWITCH, _RESET);

        // Wait for the graphics engine to be idle

        GrDone();

        HalGrLoadChannelContext(ChID);

        // Refresh interrupt status since HAL may have resolved
        // some (or all) pending conditions.

        intr = REG_RD32(regbase, NV_PGRAPH_INTR);
        if (intr == 0)
            goto done;
    }

#if DBG

    // Handle methods in software

    if (intr & DRF_DEF(_PGRAPH, _INTR, _MISSING_HW, _PENDING)) 
    {
        DXGRIP("Graphics engine software method");
        
        // Reset condition

        REG_WR_DRF_DEF(regbase, _PGRAPH, _INTR, _MISSING_HW, _RESET);
    }

    if (((intr & DRF_DEF(_PGRAPH, _INTR, _NOTIFY, _PENDING)) ||
         (intr & DRF_DEF(_PGRAPH, _INTR, _ERROR, _PENDING))) &&
        (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _ILLEGAL_MTHD, _PENDING))) 
    {
        DXGRIP("Graphics engine software method-2");
        
        // Reset condition.
        
        if (intr & DRF_DEF(_PGRAPH, _INTR, _NOTIFY, _PENDING)) {
            REG_WR_DRF_DEF(regbase, _PGRAPH, _INTR, _NOTIFY, _RESET);
        } else {
            REG_WR_DRF_DEF(regbase, _PGRAPH, _INTR, _ERROR, _RESET);
        }
    }

    // Refresh interrupt status since HAL may have resolved
    // some (or all) pending conditions.
    
    intr = REG_RD32(regbase, NV_PGRAPH_INTR);
    if (intr == 0)
        goto done;

#endif

    // Reset interrupt flags before servicing them
    
    REG_WR32(regbase, NV_PGRAPH_INTR, intr);

#if DBG
    
    // If that was the only reason we're in here, let's leave now.  Most of the object
    // status updates that follow aren't valid when we just context switching.
    
    if ((intr == DRF_DEF(_PGRAPH, _INTR, _CONTEXT_SWITCH, _PENDING)) ||
        (intr == DRF_DEF(_PGRAPH, _INTR, _SINGLE_STEP, _PENDING))) 
    {
        DXGRIP("Reset FIFO");
        
        goto done;
    }

    // Missing hardware? Must be a software method. Route accordingly.
    
    if (intr & DRF_DEF(_PGRAPH, _INTR, _MISSING_HW, _PENDING)) 
    {
        // Wait for the graphics engine to be idle

        GrDone();

        DXGRIP("MP: Missing hardware object");
    }
#endif

    // Software assisted notification
    //
    // Note: only NV10 has a PGRAPH_INTR_ERROR_PENDING bit which also flags
    // illegal method exceptions, but it should be safe to check this on NV4
    // since this'll read back as 0.

    if (NotifySource &&
        ((intr & (DRF_DEF(_PGRAPH, _INTR, _NOTIFY, _PENDING) |
                  DRF_DEF(_PGRAPH, _INTR, _ERROR, _PENDING)))))
    {
        ULONG Data = REG_RD32(regbase, NV_PGRAPH_TRAPPED_DATA_LOW);

        // Check per-class valid method.

        if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _ILLEGAL_MTHD, _PENDING))
        {        
            //BUGBUG:         
        }
        else if (Offset == NV097_NO_OPERATION)
        {
            // The method is stored in the Data portion of this notification
            // The data for the method is stored in the stencil clear value
            
            ULONG Method = Data;
            ULONG MethodData = REG_RD32(regbase, NV_PGRAPH_ZSTENCILCLEARVALUE);

            SoftwareMethod(Data, MethodData);
        }
        else
        {
            //
            // Any hardware-generated errors are FATAL, so we always spew
            // and break, even on free builds.  In checked builds, we disable the
            // interrupts so we can dump the state.
            //
#if DBG
            _disable();

            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _DATA_ERROR, _PENDING))
                ReportHardwareError("Graphics invalid data error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _PROTECTION_ERROR, _PENDING))
                ReportHardwareError("Graphics surface protection error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _RANGE_EXCEPTION, _PENDING))
                ReportHardwareError("Graphics range exception error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _LIMIT_COLOR, _PENDING))
                ReportHardwareError("Graphics color buffer limit error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _LIMIT_ZETA, _PENDING))
                ReportHardwareError("Graphics zeta buffer limit error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _ILLEGAL_MTHD, _PENDING))
                ReportHardwareError("Graphics illegal method error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _DMA_R_PROTECTION, _PENDING))
                ReportHardwareError("Graphics dma read protection error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _DMA_W_PROTECTION, _PENDING))
                ReportHardwareError("Graphics dma write protection error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _FORMAT_EXCEPTION, _PENDING))
                ReportHardwareError("Graphics format exception error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _PATCH_EXCEPTION, _PENDING))
                ReportHardwareError("Graphics patch exception error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _STATE_INVALID, _PENDING))
                ReportHardwareError("Graphics object state invalid error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _DOUBLE_NOTIFY, _PENDING))
                ReportHardwareError("Graphics double notify error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _NOTIFY_IN_USE, _PENDING))
                ReportHardwareError("Graphics notify in use error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _METHOD_CNT, _PENDING))
                ReportHardwareError("Graphics method count error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _BFR_NOTIFICATION, _PENDING))
                ReportHardwareError("Graphics buffer notification error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _DMA_VTX_PROTECTION, _PENDING))
                ReportHardwareError("Graphics DMA vertex protection error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _IDX_INLINE_REUSE, _PENDING))
                ReportHardwareError("Graphics index inline reuse error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _INVALID_OPERATION, _PENDING))
                ReportHardwareError("Graphics invalid operation error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _FD_INVALID_OP, _PENDING))
                ReportHardwareError("Graphics FD invalid operation error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _TEX_A_PROTECTION, _PENDING))
                ReportHardwareError("Graphics texture A protection error");
            if (NotifySource & DRF_DEF(_PGRAPH, _NSOURCE, _TEX_B_PROTECTION, _PENDING))
                ReportHardwareError("Graphics texture B protection error");
#endif

            ULONG Class = REG_RD_DRF(regbase, _PGRAPH, _CTX_SWITCH1, _GRCLASS);
            ULONG ChID = DRF_VAL(_PGRAPH, _TRAPPED_ADDR, _CHID, Offset);

            DbgPrint("Graphics hardware error information:\n"
                     " Source: %08x\n"
                     "   ChID: %d\n"
                     "  Class: %x\n"
                     " Method: %08x\n"
                     "   Data: %08x\n"
                     "    Get: %08x\n", 
                     NotifySource, ChID, Class, Offset, Data, 0x80000000 | *((DWORD*)0xfd003244));
#if DBG
            _enable();                
            g_DpcRIPFired = true;
#endif

            _asm int 3;
        }            
    }

#if DBG

    // Software assisted buffer notification
    
    if (intr & DRF_DEF(_PGRAPH, _INTR, _BUFFER_NOTIFY, _PENDING)) 
    {
        GrDone();

        DXGRIP("MP: Software assisted buffer notification");
    }

    if (intr & DRF_DEF(_PGRAPH, _INTR, _TLB_PRESENT_TEX_A, _PENDING)) 
    {
        WARNING("MP: Graphics DMA TLB not present or valid (A)");
    }

    if (intr & DRF_DEF(_PGRAPH, _INTR, _TLB_PRESENT_TEX_B, _PENDING)) 
    {
        WARNING("MP: Graphics DMA TLB not present or valid (B)");
    }

#endif

done:
    
    // Reset the FIFO
    
    REG_WR_DRF_DEF(regbase, _PGRAPH, _FIFO, _ACCESS, _ENABLED);
    return REG_RD32(regbase, NV_PGRAPH_INTR);
}


ULONG
CMiniport::ServiceFifoInterrupt()
{
    BYTE* regbase = (BYTE*)m_RegisterBase;
    
    ULONG intr = REG_RD32(regbase, NV_PFIFO_INTR_0);

#if DBG
    if (intr & DRF_DEF(_PFIFO, _INTR_0, _RUNOUT, _PENDING)) 
    {
        // NOTE: We don't handle runouts.  These only occur with PIO and multiple channels
        //       so we don't expect to ever see this interrupt

        DXGRIP("MP: RUNOUT NOT HANDLED\n");
    }
#endif

    // Handle semaphore errors...

    if (intr & DRF_DEF(_PFIFO, _INTR_0, _SEMAPHORE, _PENDING)) 
    {
        DXGRIP("MP: DMA semaphore pending");
        REG_WR_DRF_DEF(regbase, _PFIFO, _INTR_0, _SEMAPHORE, _RESET);
    }
    if (intr & DRF_DEF(_PFIFO, _INTR_0, _ACQUIRE_TIMEOUT, _PENDING)) 
    {
        DXGRIP("MP: DMA acquire timeout pending");
        REG_WR_DRF_DEF(regbase, _PFIFO, _INTR_0, _ACQUIRE_TIMEOUT, _RESET);
    }

    // Refresh interrupt status value in case HAL cleared any exceptions.

    intr = REG_RD32(regbase, NV_PFIFO_INTR_0);

    if (intr & DRF_DEF(_PFIFO, _INTR_0, _CACHE_ERROR, _PENDING)) 
    {
        ULONG Reason = REG_RD32(regbase, NV_PFIFO_CACHE1_PULL0);
#if DBG
        ULONG GetPtr = REG_RD32(regbase, NV_PFIFO_CACHE1_GET) >> 2;
#endif

        // Cache error handler.  We got here due to one of two issues.
        //
        // 1) The current object that was just hashed/routed is configured
        //    as a SW_ENGINE object
        // 2) There is no hash entry available for the current object
        //
                
        //
        // Disable the fifo
        //
        // Don't explicitly disable the puller -- it should already
        // be off by the time we get here.  Verify with an assert().

        REG_WR_DRF_DEF(regbase, _PFIFO, _CACHES,       _REASSIGN, _DISABLED);
        REG_WR_DRF_DEF(regbase, _PFIFO, _CACHE1_PULL0, _ACCESS,   _DISABLED);

        // Clear condition.

        REG_WR_DRF_DEF(regbase, _PFIFO, _INTR_0, _CACHE_ERROR,  _RESET);

        // Timeout

        UINT i = 0xFFFF;
        while (i && (Reason & DRF_DEF(_PFIFO, _CACHE1_PULL0, _HASH_STATE, _BUSY))) 
        {
            i--;
            Reason = REG_RD32(regbase, NV_PFIFO_CACHE1_PULL0);
        }

#if DBG
        if (Reason & DRF_DEF(_PFIFO, _CACHE1_PULL0, _HASH, _FAILED)) 
        {
            DPCRIP("MP: FIFO hash error");
        }
#endif

#if DBG
        if ((Reason & DRF_DEF(_PFIFO, _CACHE1_PULL0, _DEVICE, _SOFTWARE)) ||
            (Reason & DRF_DEF(_PFIFO, _CACHE1_PULL0, _HASH, _FAILED))) 
        {

            DPCRIP("MP: Access to free channel");

            // Advance the FIFO get pointer.

            GetPtr++;
            REG_WR_DRF_NUM(regbase, _PFIFO, _CACHE1_GET, _ADDRESS, GetPtr);
        }
#endif

        // Clear the fifo's hash result

        REG_WR32(regbase, NV_PFIFO_CACHE1_HASH, 0);

        // Re-enable cache draining.

        REG_WR_DRF_DEF(regbase, _PFIFO_, CACHE1_PULL0, _ACCESS,   _ENABLED);
        REG_WR_DRF_DEF(regbase, _PFIFO_, CACHES,       _REASSIGN, _ENABLED);
    }

#if DBG
    if (intr & DRF_DEF(_PFIFO, _INTR_0, _RUNOUT_OVERFLOW, _PENDING)) 
    {
        // NOTE: We don't handle runouts.  These only occur with PIO and multiple channels
        //       so we don't expect to ever see this interrupt

        DPCRIP("MP: RUNOUT NOT HANDLED\n");
    }
#endif

    if (intr & DRF_DEF(_PFIFO, _INTR_0, _DMA_PUSHER, _PENDING)) 
    {
#if DBG
        _disable();
        ReportHardwareError("Grahpics Parse Error");
#endif
        
        DbgPrint("Graphics Parse error information:\n"
                 "  Get: %x\n"
                 " Last: %x\n"
                 "  Cmd: %x\n",
                 0x80000000 | *((DWORD*)0xfd003244),
                 *((DWORD*)0xFD0032A4),
                 *((DWORD*)0xFD0032A8));

#if DBG
        _enable();
        g_DpcRIPFired = TRUE;
#endif
        _asm int 3;

        // create a write cycle to the FB (used as an analyzer trigger).

        //
        // We're already roached - can we just spew the above and bail?
    
        //
        // Clear condition.
        //
        REG_WR_DRF_DEF(regbase, _PFIFO, _INTR_0, _DMA_PUSHER, _RESET);
        REG_WR32(regbase, NV_PFIFO_CACHE1_DMA_STATE, 0);
        
        // The dma pusher has faulted while processing the push buffer stream.  This is
        // probably due to an invalid jump or bad opcode.
        //
        // Move the get pointer forward if necessary to skip this bad data
        
        if (REG_RD32(regbase, NV_PFIFO_CACHE1_DMA_PUT)
                != REG_RD32(regbase, NV_PFIFO_CACHE1_DMA_GET)) 
        {
            ULONG getptr;

            getptr = REG_RD32(regbase, NV_PFIFO_CACHE1_DMA_GET);
            REG_WR32(regbase, NV_PFIFO_CACHE1_DMA_GET, getptr+4);
            
            // BUGBUG: This somehow needs to account for rollover            
        }
    }

#if DBG
    if (intr & DRF_DEF(_PFIFO, _INTR_0, _DMA_PT, _PENDING)) 
    {
        // If we get this -- the fifo engine must have attempted to fetch an invalid
        // dma page.  We could attempt to reconstruct what was meant to happen, or we can
        // just barf for now.  I vote for barfing...
        
        DPCRIP("MP: Invalid DMA pusher PTE fetch");
        REG_WR_DRF_DEF(regbase, _PFIFO, _INTR_0, _DMA_PT, _RESET);
    }
#endif

    //
    // If the dma pusher is currently suspended, wait until the CACHE1 is empty
    // and then set it running again.
    //
    if (REG_RD_DRF(regbase, _PFIFO, _CACHE1_DMA_PUSH, _STATUS) ==
             NV_PFIFO_CACHE1_DMA_PUSH_STATUS_SUSPENDED) {
        while (REG_RD_DRF(regbase, _PFIFO, _CACHE1_STATUS, _LOW_MARK) !=
                 NV_PFIFO_CACHE1_STATUS_LOW_MARK_EMPTY) 
        {
            // If there's a pending FIFO intr, make this check after servicing it

            if (REG_RD32(regbase, NV_PFIFO_INTR_0))
                break;

            // Check if GE needs servicing

            if (REG_RD32(regbase, NV_PGRAPH_INTR)) 
            {
                ServiceGrInterrupt();
            }

            // Check if vblank needs servicing (for NV15 HW flip)

            if (REG_RD32(regbase, NV_PMC_INTR_0) & DRF_DEF(_PMC, _INTR_0, _PCRTC, _PENDING)) 
            {
                VBlank();
            }
        }

        if (REG_RD_DRF(regbase, _PFIFO, _CACHE1_STATUS, _LOW_MARK) ==
                NV_PFIFO_CACHE1_STATUS_LOW_MARK_EMPTY) 
        {
            while (REG_RD_DRF(regbase, _PFIFO, _CACHES, _DMA_SUSPEND) ==
                    NV_PFIFO_CACHES_DMA_SUSPEND_BUSY) 
            {
            }

            FLD_WR_DRF_DEF(regbase, _PFIFO, _CACHE1_DMA_PUSH, _STATUS, _RUNNING);
        }
    }

    
    // Re-enable FIFO CACHE1 if all conditions cleared.
    
    if (REG_RD32(regbase, NV_PFIFO_INTR_0) == 0) 
    {
        REG_WR_DRF_DEF(regbase, _PFIFO_, CACHE1_PULL0, _ACCESS, _ENABLED);
        REG_WR_DRF_DEF(regbase, _PFIFO_, CACHES, _REASSIGN, _ENABLED);
    }

    
    // See if there is yet another FIFO exception to be handled.
    // Just need to make sure that "intr" is non-zero in order to
    // get more servicing done.
    
    intr = REG_RD32(regbase, NV_PFIFO_INTR_0);
    intr |= REG_RD_DRF(regbase, _PFIFO, _DEBUG_0, _CACHE_ERROR0);
    return intr;
}


ULONG
CMiniport::ServiceMediaPortInterrupt()
{
    DPCRIP("MP: Unhandled ServiceMediaPortInterrupt");

    return 0;
}


ULONG
CMiniport::ServiceVideoInterrupt()
{
    BYTE* reg = (BYTE*)m_RegisterBase;

     
    // This is overlay goop
    
    ULONG intr = REG_RD32(reg, NV_PVIDEO_INTR);
    
    if (intr & DRF_NUM(_PVIDEO, _INTR, _BUFFER_0, NV_PVIDEO_INTR_BUFFER_0_PENDING)) 
    {
        REG_WR_DRF_NUM(reg, _PVIDEO, _INTR, _BUFFER_0, NV_PVIDEO_INTR_BUFFER_0_RESET);
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Applies fixups to a push-buffer.
//
// 'Call Long' is broken on the NV2A, so we have to emulate callable push-
// buffers by patching the return address ourselves when the push-buffer is
// called.  We also provide a general mechanism for doing per-call fix-ups
// for Transform data and the like.

VOID
CMiniport::FixupPushBuffer(
    PUSHBUFFERFIXUPINFO *pPushBufferFixupInfo, // WARNING: This is write-combined!
    ULONG Method
    )
{
    ASSERT((Method == NVX_PUSH_BUFFER_RUN) || (Method == NVX_PUSH_BUFFER_FIXUP));

    DWORD Size;
    DWORD* pFixupData = pPushBufferFixupInfo->pFixupData;
    BYTE* pPushBuffer = pPushBufferFixupInfo->pStart;

    //
    // Apply the user-specified fix-up stream.  Note that the caller is
    // responsible for ensuring that the fix-up data is still allocated
    // and valid when we get to this point.
    //
    if (pFixupData != NULL)
    {
        DWORD PreviousOffset = 0;
        while ((Size = *pFixupData) != 0xffffffff)
        {
            DWORD Offset = *(pFixupData + 1);

            if ((Offset & 3) || (Size & 3) || (Offset < PreviousOffset))
            {
                DPCRIP("MP: Bad RunPushBuffer fix-up data (got overwritten?)");
            }

            PreviousOffset = Offset;

            pFixupData += 2;
            memcpy(pPushBuffer + Offset, pFixupData, Size);
            pFixupData = (DWORD*) ((BYTE*) pFixupData + Size);
        }
    }

    //
    // Fix-up the return address.
    //
    DWORD ReturnOffset = pPushBufferFixupInfo->ReturnOffset;
    DWORD ReturnAddress 
        = GetGPUAddressFromWC((VOID*) pPushBufferFixupInfo->ReturnAddress);

    *((DWORD*) (pPushBuffer + ReturnOffset)) 
        = PUSHER_JUMP(ReturnAddress);

    //
    // Let the pusher code know how long this push-buffer is, and where
    // in the main push-buffer it will return.  We don't do this for
    // push-buffer resources called from within push-buffer resources,
    // because the pusher code would get confused.
    //
    if (Method == NVX_PUSH_BUFFER_RUN)
    {
        //
        // The NV2A has a bug that prevents us from ever doing a FIFO 'return'
        // instruction.  To allow us to do a FIFO 'call' instruction again we 
        // have to clear the 'NV_PFIFO_CACHE1_DMA_SUBROUTINE_STATE_ACTIVE'
        // bit.  We of course don't want to do a read-modify-write on this
        // register (because register reads are very expensive), so we just
        // write the return-address to the register, which nicely clears the
        // active bit:
        //
        ASSERT(!(ReturnAddress & NV_PFIFO_CACHE1_DMA_SUBROUTINE_STATE_ACTIVE));

        REG_WR32(m_RegisterBase, NV_PFIFO_CACHE1_DMA_SUBROUTINE, ReturnAddress);

        m_PusherGetRunTotal += ReturnOffset;
    }
}

//-----------------------------------------------------------------------------
// Decode all software method instructions

VOID
CMiniport::SoftwareMethod(
    ULONG Method, ULONG Data
    )
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;
    ULONG RampNo;

    switch (Method) 
    {
    case NVX_FLIP_IMMEDIATE:
        {
            ASSERT((Data & 3) == 0);

            // Program the hardware.
            DacProgramVideoStart(Data);

            //
            // Process gamma ramps.  We choose between the two gamma ramps based 
            // on the LSB of the current flip count
            //
            RampNo = (m_VBlankFlipCount & 1);
            if (m_GammaUpdated[RampNo] == TRUE)
            {
                DacProgramGammaRamp(&(m_GammaRamp[RampNo]));
                m_GammaUpdated[RampNo] = FALSE;
            }
    
            m_VBlankFlipCount++;
            m_FlipCount++;
    
            if (m_pSwapCallback)
            {
                D3DSWAPDATA data;
    
                ZeroMemory(&data, sizeof(data));
                data.Swap = m_FlipCount;
    
                m_pSwapCallback(&data);
            }
        }
        break;

    case NVX_FLIP_SYNCHRONIZED:
        {
            DWORD flipVBlank;
            DWORD timeOfNextFlip;

            DWORD flipIndex = m_FlipCount & (MAX_QUEUED_FLIPS - 1);

            m_FlipCount++;

            INT missedVBlanks = (m_VBlankCount + 1 - m_VBlankCountAtLastFlip) - m_VBlanksBetweenFlips;
            if (missedVBlanks > 0)
            {
                // We've missed some frames, so we know this flip will be
                // handled on the very next VBlank.
                //
                flipVBlank = m_VBlankCount + 1;
                timeOfNextFlip = m_TimeOfLastVBlank + m_TimeBetweenVBlanks;
            }
            else
            {
                // Because of the flip queue, this flip may not actually happen
                // for several VBlanks.
                //
                DWORD flipQueueLength = m_FlipCount - m_VBlankFlipCount;

                ASSERT((flipQueueLength > 0) && (flipQueueLength <= MAX_QUEUED_FLIPS));

                flipVBlank = m_VBlankCountAtLastFlip + flipQueueLength * m_VBlanksBetweenFlips;
                timeOfNextFlip = m_TimeOfLastFlip + m_TimeBetweenVBlanks * flipQueueLength * m_VBlanksBetweenFlips;
                missedVBlanks = 0;
            }

            // We can't make any time estimates before at least two VBlanks have
            // happened.
            //
            INT timeUntilFlipVBlank = timeOfNextFlip - GetTime();

            // Our time estimate can go negative if something weird happened
            // to throw timing off to our DPC, such as breaking into the
            // debugger.  But the VBlank hasn't been handled yet, so we know
            // the value isn't really zero or less.
            //
            if (timeUntilFlipVBlank < 0)
                timeUntilFlipVBlank = 1;

            // If m_TimeBetweenVBlanks is zero, we haven't yet had a chance to
            // time how long it takes between vertical blanks - so we obviously
            // can't make a time estimate.
            //
            if (m_TimeBetweenVBlanks == 0)
                timeUntilFlipVBlank = 0;

            if ((missedVBlanks > 0) && (m_OrImmediate))
            {
                // We missed a frame, so process the flip immediately (can
                // cause tearing but reduces timing glitches).
                //
                // Pretend the flip happened at the last VBlank, not the next.
                //
                VBlankFlip(Data, m_TimeOfLastVBlank);

                flipVBlank--;         
                timeUntilFlipVBlank = 0;
            }
            else
            {
                // Queue the flip to be handled at Vblank.
                //
                ASSERT(m_VBlankFlips[flipIndex].Pending == FALSE);
        
                m_VBlankFlips[flipIndex].Pending = TRUE;
                m_VBlankFlips[flipIndex].Offset = Data;
            }

            // Handle the Swap call-back.
            //
            if (m_pSwapCallback)
            {
                D3DSWAPDATA data;
    
                data.Swap = m_FlipCount;
                data.MissedVBlanks = missedVBlanks;
                data.SwapVBlank = flipVBlank;
                data.TimeUntilSwapVBlank = timeUntilFlipVBlank;
                data.TimeBetweenSwapVBlanks = m_TimeBetweenVBlanks * m_VBlanksBetweenFlips;
    
                m_pSwapCallback(&data);
            }
        }
        break;

    case NVX_PUSH_BUFFER_RUN:
    case NVX_PUSH_BUFFER_FIXUP:
        {
            FixupPushBuffer((PUSHBUFFERFIXUPINFO*) Data, Method);
    
            //
            // Make sure our modifications get written to memory.  Note that 
            // FlushWCCache is okay to be called even from a DPC.
            //
            FlushWCCache();
        }
        break;

    case NVX_FENCE:

        // Activate the fence.
        KeSetEvent(&m_BusyBlockEvent, 1, FALSE);
        break;

    case NVX_WRITE_CALLBACK:

        // Note that we should have already done an NV097_WAIT_FOR_IDLE.
        ASSERT(!REG_RD32(m_RegisterBase, NV_PGRAPH_STATUS));

        // fall through

    case NVX_READ_CALLBACK:
        {
            D3DCALLBACK pCallback = (D3DCALLBACK)Data;
    
            DWORD Context = REG_RD32(RegisterBase, NV_PGRAPH_COLORCLEARVALUE);
    
            pCallback(Context);
        }
        break;

    case NVX_DXT1_NOISE_ENABLE:

        // Note that we should have already done an NV097_WAIT_FOR_IDLE.
        ASSERT(!REG_RD32(m_RegisterBase, NV_PGRAPH_STATUS));

        RDI_REG_WR32(RegisterBase, NV_IGRAPH_TC_TPA_INDEX_SELECT,
                     NV_IGRAPH_TC_TPA_DXTDITH_INDEX_ADDRESS,
                     Data);

        RDI_REG_WR32(RegisterBase, NV_IGRAPH_TC_DXP_INDEX_SELECT,
                     NV_IGRAPH_TC_DXP_DXTDITH_INDEX_ADDRESS,
                     Data);

        break;

    case NVX_WRITE_REGISTER_VALUE:
    {
        // This method can assume that the back-end is idle.
        ASSERT(!REG_RD32(m_RegisterBase, NV_PGRAPH_STATUS));

        DWORD WriteRegisterOffset = Data;
        DWORD Value = REG_RD32(RegisterBase, NV_PGRAPH_COLORCLEARVALUE);

        REG_WR32(RegisterBase, WriteRegisterOffset, Value);

        break;
    }

    default:
        NODEFAULT("MP: Unhandled SoftwareMethod\n");

#if DBG
        _disable();
        ReportHardwareError("Unhandled Software Method");

        DbgPrint("Unhandled Software Method information:\n"
                 " Method: 0x%08lx\n"
                 "   Data: 0x%08lx\n", Method, Data);
        _enable();
        g_DpcRIPFired = true;

        _asm int 3;

        break;
#endif

    }

}

VOID
CMiniport::ShutdownNotification(
    IN PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration
    )
{
    CMiniport* miniport = CONTAINING_RECORD(ShutdownRegistration, CMiniport,
        m_ShutdownRegistration);

    BYTE* regbase = (BYTE*)(miniport->m_RegisterBase);

    // I'm using this directly to avoid calling back into the kernel which could
    // do all sorts of nasty stuff at a time that we're trying to be simple and 
    // quick.  If this hasn't been set up yet then we haven't set the overscan
    // color and there is no reason to do any of this.
    //
    if (D3D__AvInfo)
    {
        if ((D3D__AvInfo & AV_PACK_MASK) == AV_PACK_SCART)
        {
            REG_WR32(regbase, NV_PRAMDAC_FP_INACTIVE_PXL_COLOR, 0x00000000);
        }
        else
        {
            REG_WR32(regbase, NV_PRAMDAC_FP_INACTIVE_PXL_COLOR, 0x00800080);
        }
    }

    miniport->DisableInterrupts(regbase);
    miniport->ShutdownEngines();
}


#if DBG

//-----------------------------------------------------------------------------
// Reports an graphics hardware error message.  Only report fatal errors using
// this function because it disables some hardware and we cannot recover from
// this state.
//
VOID
ReportHardwareError(
    CONST CHAR* Message
    )
{
    ULONG Address;
    ULONG i;
    static ULONG ErrorCount;

    //
    // We handle multiple errors by just printing the error message but we
    // dump the state only while printing the first error message
    //

    ErrorCount++;
    if (ErrorCount > 1) {
        DbgPrint("\nError %d: %s\n\n", ErrorCount, Message);
        return;        
    }

    //
    // Print header information
    // 

    WARNING("");
    DbgPrint("Graphics Hardware Error\n"
             "-----------------------\n");

    DbgPrint("Graphics hardware has detected an error.  Please copy the information\n"
             "provided below and email it to support at xboxds@xbox.com.\n");

    if (!D3D__SingleStepPusher)
    {
        DbgPrint("\nNOTE: To pinpoint the API call that may have caused this error, you may\n"
                 "also re-run with the global variable 'D3D__SingleStepPusher' set to 1.\n");
    }

    //
    // Print the error message
    //

    DbgPrint("\nError: %s\n", Message);

    DbgPrint("\nPush buffer: HWPut=%x  HWGet=%x  SWPut=%x\n", 
        *((DWORD*)0xfd003240), *((DWORD*)0xfd003244), g_pDevice->m_Pusher.m_pPut);
    
    //
    // Dump out the CACHE1, internal PGRAPH fifo and other information.  In order 
    // to do that we need to disable the puller first
    //
    
    *((DWORD*)0xfd003220) = 0;
    *((DWORD*)0xfd003250) = 0;

    Address = 0xfd003800;
    
    DbgPrint("\nC1Get=%x  C1Put=%x ->", *((DWORD*)0xfd003270), *((DWORD*)0xfd003210));
    for (i = 0; i < 0x140; i++) 
    {
        if (i % 8 == 0) 
        {
            DbgPrint("\n%04x:", Address & 0x0000FFFF);   
        }

        DbgPrint(" %08x", *((DWORD*)Address));

        Address += 4;

        if (Address == 0xfd003c00) {
            DbgPrint("\n400700->");
            Address = 0xfd400700;    

        } else if (Address == 0xfd400780) {
            DbgPrint("\n4007a0->");
            Address = 0xfd4007a0;                        
        }
    }
    DbgPrint("\n");
    
}

#endif

} // end of namespace



