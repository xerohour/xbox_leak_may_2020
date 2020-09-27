/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mpcore.cpp
 *  Content:    Hardware initialization
 *
 ***************************************************************************/


#include "precomp.hpp"

#include <ntos.h>
#include <pci.h>

extern "C" PVOID *GpuInstanceMemory;


// Optimize this module for size (favor code space)

#pragma optimize("s", on)


#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{


#define GR_SAVE_STATE(Ptr, misc)    {(misc) = REG_RD32(Ptr, NV_PGRAPH_FIFO);\
                                     REG_WR32(Ptr, NV_PGRAPH_FIFO,        \
                                     DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _DISABLED));}

#define GR_RESTORE_STATE(Ptr, misc) REG_WR32(RegisterBase, NV_PGRAPH_FIFO, misc)


VOID
CMiniport::HalMcControlInit()
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;
    ULONG M, N, P, O;

    m_HalInfo.McSave = REG_RD32(RegisterBase, NV_PMC_ENABLE);
    if (m_HalInfo.McSave == 0)
    {
        // 
        // Reset all devices and interrupts
        //

        REG_WR32(RegisterBase, NV_PMC_ENABLE, 0x00000000);
    }

    m_HalInfo.McSaveIntrEn0 = REG_RD32(RegisterBase, NV_PMC_INTR_EN_0);
    REG_WR32(RegisterBase, NV_PMC_ENABLE, 0xFFFFFFFF);


    M = REG_RD_DRF(RegisterBase, _PRAMDAC, _MPLL_COEFF, _MDIV);
    N = REG_RD_DRF(RegisterBase, _PRAMDAC, _MPLL_COEFF, _NDIV);
    P = REG_RD_DRF(RegisterBase, _PRAMDAC, _MPLL_COEFF, _PDIV);
    O = 1;

    m_DacInfo.MPllM = M;
    m_DacInfo.MPllN = N;
    m_DacInfo.MPllO = O;
    m_DacInfo.MPllP = P;
    m_DacInfo.MClk  = ( M != 0 ) ? (N * m_GenInfo.CrystalFreq / (1 << P) / M) : 0;

    M = REG_RD_DRF(RegisterBase, _PRAMDAC, _VPLL_COEFF, _MDIV);
    N = REG_RD_DRF(RegisterBase, _PRAMDAC, _VPLL_COEFF, _NDIV);
    P = REG_RD_DRF(RegisterBase, _PRAMDAC, _VPLL_COEFF, _PDIV);
    O = 1;

    m_DacInfo.VPllM = M;
    m_DacInfo.VPllN = N;
    m_DacInfo.VPllO = O;
    m_DacInfo.VPllP = P;
    m_DacInfo.VClk  = ( M != 0 ) ? (N * m_GenInfo.CrystalFreq / (1 << P) / M) : 0;

    M = REG_RD_DRF(RegisterBase, _PRAMDAC, _NVPLL_COEFF, _MDIV);
    N = REG_RD_DRF(RegisterBase, _PRAMDAC, _NVPLL_COEFF, _NDIV);
    P = REG_RD_DRF(RegisterBase, _PRAMDAC, _NVPLL_COEFF, _PDIV);
    O = 1;

    m_DacInfo.NVPllM = M;
    m_DacInfo.NVPllN = N;
    m_DacInfo.NVPllO = O;
    m_DacInfo.NVPllP = P;
    m_DacInfo.NVClk  = ( M != 0 ) ? (N * m_GenInfo.CrystalFreq / (1 << P) / M) : 0;

}


VOID
CMiniport::HalFbControlInit()
{
    BYTE* regbase = (BYTE*)m_RegisterBase;

    // 
    // Save off chip-dependent fb state.
    //
    
    m_HalInfo.FbSave0 = REG_RD32(regbase, NV_PFB_CFG0);
    m_HalInfo.FbSave1 = REG_RD32(regbase, NV_PFB_CFG1);

    //
    // Set ram type
    //
    
    if (REG_RD_DRF(regbase, _PBUS, _FBIO_RAM, _TYPE) == NV_PBUS_FBIO_RAM_TYPE_SDR)
    {
        m_GenInfo.VideoRamType = BUFFER_SDRAM;
    }
    else
    {
        m_GenInfo.VideoRamType = BUFFER_DDRAM;
    }

    //
    // Verify partition count.
    //    0=1partition, 1=2partitions, 3=4partitions (default).
    //

    ASSERT(REG_RD_DRF(regbase, _PFB, _CFG0, _PART) == 3);

    //
    // Claim how much space we're going to use for instance memory.  For the
    // startup animation case, we don't want the memory manager to free the
    // existing reservation, so we specify MAXULONG_PTR.  The memory manager
    // returns the number of bytes that are reserved for other use from the top
    // of memory through InstPadSize.
    //

    ULONG InstPadSize;

#ifdef STARTUPANIMATION
    m_InstMem = MmClaimGpuInstanceMemory(MAXULONG_PTR, &InstPadSize);
#else
    m_InstMem = MmClaimGpuInstanceMemory(INSTANCE_MEM_MAXSIZE, &InstPadSize);
#endif
    ASSERT(m_InstMem != NULL);

    m_InstMem = (PBYTE)m_InstMem - INSTANCE_MEM_MAXSIZE;

    //
    // The start of all of these space is after the BIOS
    //
    
    ULONG tmpAddr = PRAMIN_OFFSET + InstPadSize;

    //
    // Configure hash table to be 4K
    //
    
    m_HalInfo.HashTableAddr = tmpAddr;
    tmpAddr += 0x1000;

    REG_WR32(regbase, NV_PFIFO_RAMHT,
        (DRF_DEF(_PFIFO, _RAMHT, _SIZE, _4K) |
         DRF_NUM(_PFIFO, _RAMHT, _BASE_ADDRESS, InstPadSize >> 12) |
         DRF_DEF(_PFIFO, _RAMHT, _SEARCH, _128)));
    
    // 
    // NV20 has two distinct context areas.
    //
    
    m_HalInfo.FifoContextAddr1 = tmpAddr;
    tmpAddr += NUM_FIFOS_NV20 * FC1_SIZE_NV20;
    m_HalInfo.FifoContextAddr2 = tmpAddr;
    tmpAddr += NUM_FIFOS_NV20 * FC2_SIZE_NV20;

    ULONG addr1 = m_HalInfo.FifoContextAddr1 - PRAMIN_OFFSET;
    ULONG addr2 = m_HalInfo.FifoContextAddr2 - PRAMIN_OFFSET;

    REG_WR32(regbase, NV_PFIFO_RAMFC,
        (DRF_NUM(_PFIFO, _RAMFC, _BASE_ADDRESS, (addr1 >> FC_MIN_ALIGN_SHIFT)) |
         DRF_DEF(_PFIFO, _RAMFC, _SIZE, _2K) |
         DRF_NUM(_PFIFO, _RAMFC, _BASE_ADDRESS2, (addr2>> FC_MIN_ALIGN_SHIFT))));

    //
    // Sum of all allocations so far
    //
    
    m_FreeInstAddr = ((tmpAddr) - PRAMIN_OFFSET) >> 4;

    // 
    // Enable writes by disabling write protection
    //

    FLD_WR_DRF_DEF(regbase, _PFB, _NVM, _MODE, _DISABLE);

    // 
    // Reserve high block of fb mem as instance space.
    //
    
    for (addr1 = 0; addr1 < (INSTANCE_MEM_MAXSIZE) / 4; addr1++)
    {
        REG_WR32(regbase, NV_PRAMIN_DATA032(addr1) + InstPadSize, 0);
    }

    //
    // GR area
    //
    
    m_HalInfo.GrCtxTableBase = ReserveInstMem(8);
}



VOID
CMiniport::HalDacControlInit()
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;
    BYTE lock;
    ULONG data32;
    ULONG crtcAddr = 0;

    //
    // Unlock CRTC extended regs
    //
    
    HAL_CRTC_RD(RegisterBase, NV_CIO_SR_LOCK_INDEX, lock, crtcAddr);
    HAL_CRTC_WR(RegisterBase, NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, crtcAddr);

    HAL_CRTC_RD(RegisterBase, NV_CIO_CRE_TVOUT_LATENCY, data32, crtcAddr);
    data32 += 4;
    HAL_CRTC_WR(RegisterBase, NV_CIO_CRE_TVOUT_LATENCY, (BYTE)data32, crtcAddr);  //BUGBUG: truncation

    //
    // Overlay stuff
    //
    
    FLD_WR_DRF_NUM(RegisterBase, _PVIDEO, _DEBUG_2, _BURST1, 16);         // 512 bytes
    FLD_WR_DRF_NUM(RegisterBase, _PVIDEO, _DEBUG_2, _BURST2, 16);         // 512 bytes
    FLD_WR_DRF_NUM(RegisterBase, _PVIDEO, _DEBUG_3, _WATER_MARK1, 64);    // 1024 bytes
    FLD_WR_DRF_NUM(RegisterBase, _PVIDEO, _DEBUG_3, _WATER_MARK2, 64);    // 1024 bytes

    HAL_CRTC_WR(RegisterBase, NV_CIO_CRE_FFLWM__INDEX, 41, crtcAddr);
    HAL_CRTC_WR(RegisterBase, NV_CIO_CRE_FF_INDEX, 5, crtcAddr);

    //
    // Load the start address every hsync...
    //
    
    FLD_WR_DRF_DEF(RegisterBase, _PCRTC, _CONFIG, _START_ADDRESS, _HSYNC);

    //
    // Relock if necessary...
    //
    
    if (lock == 0)
    {
        HAL_CRTC_WR(RegisterBase, NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, crtcAddr);
    }
}


VOID
CMiniport::HalVideoControlInit()
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;

    //
    // Establish default register settings since we
    // don't use the videoInit_NV10 routine when doing
    // buffer kickoffs for these classes.
    //
    
    for (INT i = 0; i < 2; i++)
    {
        REG_WR32(RegisterBase, NV_PVIDEO_LUMINANCE(i),
                 DRF_DEF(_PVIDEO, _LUMINANCE, _CONTRAST, _UNITY) |
                 DRF_DEF(_PVIDEO, _LUMINANCE, _BRIGHTNESS, _UNITY));
        REG_WR32(RegisterBase, NV_PVIDEO_CHROMINANCE(i),
                 DRF_DEF(_PVIDEO, _CHROMINANCE, _SAT_COS, _UNITY) |
                 DRF_DEF(_PVIDEO, _CHROMINANCE, _SAT_SIN, _UNITY));
        REG_WR32(RegisterBase, NV_PVIDEO_DS_DX(i),
                 DRF_DEF(_PVIDEO, _DS_DX, _RATIO, _UNITY));
        REG_WR32(RegisterBase, NV_PVIDEO_DT_DY(i),
                 DRF_DEF(_PVIDEO, _DT_DY, _RATIO, _UNITY));
        REG_WR32(RegisterBase, NV_PVIDEO_POINT_IN(i), 0);
        REG_WR32(RegisterBase, NV_PVIDEO_SIZE_IN(i), 0xFFFFFFFF);
    }
}



VOID
CMiniport::HalGrControlInit()
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;
    ULONG CtxTableBase = m_HalInfo.GrCtxTableBase;
    
    //
    // Initialize graphics channel context table register.
    //
    
    REG_WR_DRF_NUM(RegisterBase, _PGRAPH, _CHANNEL_CTX_TABLE, _INST, CtxTableBase);
    for (INT i = 0; i < NUM_FIFOS_NV20; i++)
    {
        INST_WR32(RegisterBase, CtxTableBase, i * 4, 0);
    }

}


VOID
CMiniport::HalFifoControlInit()
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;

    //
    // Setup defaults
    //
    
    m_HalInfo.FifoRetryCount = 255;
    m_HalInfo.FifoUserBase = DEVICE_BASE(NV_USER);

    //
    // Setup interrupt enable mask.
    //
    
    m_HalInfo.FifoIntrEn0 =
        DRF_DEF(_PFIFO, _INTR_EN_0, _CACHE_ERROR, _ENABLED) |
        DRF_DEF(_PFIFO, _INTR_EN_0, _RUNOUT, _ENABLED) |
        DRF_DEF(_PFIFO, _INTR_EN_0, _RUNOUT_OVERFLOW, _ENABLED) |
        DRF_DEF(_PFIFO, _INTR_EN_0, _DMA_PUSHER, _ENABLED) |
        DRF_DEF(_PFIFO, _INTR_EN_0, _DMA_PT, _ENABLED) |
        DRF_DEF(_PFIFO, _INTR_EN_0, _SEMAPHORE, _ENABLED) |
        DRF_DEF(_PFIFO, _INTR_EN_0, _ACQUIRE_TIMEOUT, _ENABLED);

    //
    // Zero FIFO get/put.
    //
    
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_PUT, 0);
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_GET, 0);
    
    //
    // Zero DMA FIFO get/put.
    //
    
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_DMA_PUT, 0);
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_DMA_GET, 0);
    
    //
    // Zero FIFO hash valid.
    //
    
    REG_WR32(RegisterBase, NV_PFIFO_CACHE0_HASH, 0);
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_HASH, 0);

    //
    // Make all FIFO's default
    //
    
    REG_WR32(RegisterBase, NV_PFIFO_MODE, m_HalInfo.FifoMode);
    REG_WR32(RegisterBase, NV_PFIFO_DMA, 0);
    REG_WR32(RegisterBase, NV_PFIFO_SIZE, 0);

    //
    // Clear all pusher state
    //
    
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_DMA_STATE, 0);

    //
    // Zero run-out pointers.
    //
    
    REG_WR_DRF_NUM(RegisterBase, _PFIFO, _RUNOUT_PUT, _ADDRESS, 0);
    REG_WR_DRF_NUM(RegisterBase, _PFIFO, _RUNOUT_GET, _ADDRESS, 0);
}



VOID
CMiniport::HalGrControlLoad()
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;
    ULONG misc;

    //
    // To reset graphics on nv20, we can't use DEBUG_0, because the _CACHE_STATE 
    // bit will cause a hang in the tcache. Instead, we use the PMC register.
    //
    
    FLD_WR_DRF_DEF(RegisterBase, _PMC, _ENABLE, _PGRAPH, _DISABLED);
    FLD_WR_DRF_DEF(RegisterBase, _PMC, _ENABLE, _PGRAPH, _ENABLED);


    //
    // Initialize graphics debug values.
    //

    m_DebugRegister[0] = 0x0;

    m_DebugRegister[1] = (ULONG)
        DRF_DEF(_PGRAPH, _DEBUG_1, _VOLATILE_RESET,   _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_1, _VTX_PTE,          _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_1, _VTX_CACHE,        _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_1, _VTX_FILE,         _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_1, _DRAWDIR_AUTO,     _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_1, _DRAWDIR_Y,        _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_1, _INSTANCE,         _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_1, _CTX,              _INIT);

    m_DebugRegister[2] = (ULONG)
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_PITCH_CHECK_2D, _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_PITCH_CHECK_BLT, _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_PITCH_CHECK_CELS, _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_PITCH_CHECK_3D, _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_LIMIT_CHECK,    _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_OVRFLW_CHECK,   _ENABLED) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_MEMSIZE_CHECK,  _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_SWIZZLE_CHECK,  _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_TILEVIOL,       _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_ENDIAN_CHECK,   _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_LIMIT_INT,      _ENABLED) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_OVRFLW_INT,     _ENABLED) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_PASS_VIOL,      _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_PITCHWRAP,      _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_DITHER_3D,      _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_DITHER_2D,      _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_SWALLOW_REQS,   _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_SBFILTER,       _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_FASTCLEAR,      _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_FLUSH_HOLDOFF,  _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_2, _PREROP_TILEBIT_UPDATE, _INIT);

    m_DebugRegister[3] = (ULONG)
        DRF_DEF(_PGRAPH, _DEBUG_3, _FLUSHING,              _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _HW_CONTEXT_SWITCH,     _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _FD_CHECK,              _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _FAST_DATA_STRTCH,      _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _FAST_3D_SHADOW_DATA,   _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _CHECK_64BYTE_ALIGN,    _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _SINGLE_STEP,           _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _RDI_IDLE_WAIT,         _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _IDLE_FILTER,           _ENABLED) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _SYNCHRONIZE,           _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _OBJECT_RELOAD,         _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _PM_TRIGGER,            _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _ALTARCH,               _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _SINGLE_CYCLE_LOAD,     _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _BILINEAR_3D,           _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _VOLATILE_RESET,        _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _DATA_CHECK,            _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _DATA_CHECK_FAIL,       _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _FORMAT_CHECK,          _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _DMA_CHECK,             _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _STATE_CHECK,           _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _KELVIN_HWFLIP,         _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _FAST_3D_RESTORE,       _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _STATE3D_CHECK,         _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _XFMODE_COALESCE,       _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _CTX_METHODS,           _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _OP_METHODS,            _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_3, _IGNORE_PATCHVALID,     _INIT);

    //
    // Have to use hybrid context switch mode to work around
    // FD bug...we'll do this here so it's easier to control
    // on future chips where we'll hopefully be able to
    // employ full hw context switching.
    //
    m_DebugRegister[3] &= ~(ULONG)DRF_DEF(_PGRAPH, _DEBUG_3, _HW_CONTEXT_SWITCH, _ENABLED);

    // all DEBUG_[4,5] bits are SPARES currently
    m_DebugRegister[4] = 0x0;
    
    m_DebugRegister[5] =
        DRF_DEF(_PGRAPH, _DEBUG_5, _ZCULL_REQ_FULL_CVG,    _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_5, _ZCULL_PUNT_FB_BUSY,    _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_5, _ZCULL_PUNT_S_ON_Z,     _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_5, _ZCULL_RETURN_COMP,     _INIT);

    m_DebugRegister[6] = (ULONG)
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_SEP_ZC_READS,      _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_DITHER_3D,         _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_DITHER_2D,         _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_EARLY_ZABORT,      _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_PASS_ROPFLUSH,     _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_CBUF_DRAIN,        _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_CBUF_HIGHWATER,    _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_CBUF_HIGHWATER2,   _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_COALESCE_3D,       _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_COALESCE_2D,       _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_FIXED_ADRS,        _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_FAST_KEEP_DST,     _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_FORCE_CREAD,       _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_ALLOW_3D_SKIP_READ, _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_ZCMP_ALWAYS_READ,  _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_ZCOMPRESS_EN,      _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_BURST_CREADS,      _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_BURST_CWRITES,     _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_AUTO_INIT,         _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_HASH_TEST1,        _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_INTERLOCK,         _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_ZREAD,             _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_ZCULL_DATA,        _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_CROP_SWALLOW,      _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_6, _SPARE_BIT31,           _INIT);

    //
    // Separate color and Z reads on SDR memory configurations
    // (i.e. same dram bank).
    //
    if (REG_RD_DRF(RegisterBase, _PBUS, _FBIO_RAM, _TYPE) == NV_PBUS_FBIO_RAM_TYPE_SDR)
        m_DebugRegister[6] |= DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_SEP_ZC_READS, _ENABLED);
    else
        m_DebugRegister[6] |= DRF_DEF(_PGRAPH, _DEBUG_6, _ROP_SEP_ZC_READS, _DISABLED);

    m_DebugRegister[7] = (ULONG)
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_INIT_ZRD_TMSTAMP,  _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZCULL_NO_STALE,    _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZROP_INTERLOCK,    _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_FORCE_VISIBLE, _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_PURGE_PER_PKT, _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_FORCE_NO_16BYTE_WR, _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_FORCE_FSTCLR_EXPAND, _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_FORCE_NO_RMW_DISABLE, _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_INHIBIT_COMPRESS, _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_CMPR_FULL_BE_ONLY, _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_ALWAYS_WR_STENCIL, _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_INHIBIT_SKIP_WR_EQ, _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_SELECTPM,      _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_EXTRA1,        _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_EXTRA2,        _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_EXTRA3,        _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_CROP_SELECTPM,     _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZWR_EXTRA4,        _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_ZRD_HASH_TAGS,     _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_7, _ROP_CROP_BLIT_HASH,    _INIT);
     
    m_DebugRegister[8] = (ULONG)
        DRF_DEF(_PGRAPH, _DEBUG_8, _SHADER_FIFO_WATERMARK, _INIT) |
        DRF_DEF(_PGRAPH, _DEBUG_8, _COMBINER_NEG_TEXTURE,  _INIT);

    m_DebugRegister[9] = (ULONG)
        DRF_DEF(_PGRAPH, _DEBUG_9, _BEGIN_PATCH0_CHECK, _ENABLE) | 
        DRF_DEF(_PGRAPH, _DEBUG_9, _BEGIN_PATCH1_CHECK, _ENABLE) | 
        DRF_DEF(_PGRAPH, _DEBUG_9, _BEGIN_PATCH2_CHECK, _ENABLE) | 
        DRF_DEF(_PGRAPH, _DEBUG_9, _BEGIN_PATCH3_CHECK, _ENABLE) | 
        DRF_DEF(_PGRAPH, _DEBUG_9, _END_PATCH_CHECK, _ENABLE) | 
        DRF_DEF(_PGRAPH, _DEBUG_9, _BEGIN_END_SWATCH_CHECK, _ENABLE) | 
        DRF_DEF(_PGRAPH, _DEBUG_9, _BEGIN_END_CURVE_CHECK, _DISABLE) | 
        DRF_DEF(_PGRAPH, _DEBUG_9, _CURVE_COEFF_CHECK, _ENABLE) | 
        DRF_DEF(_PGRAPH, _DEBUG_9, _BEGIN_TRANS0_CHECK, _ENABLE) | 
        DRF_DEF(_PGRAPH, _DEBUG_9, _BEGIN_TRANS1_CHECK, _ENABLE) | 
        DRF_DEF(_PGRAPH, _DEBUG_9, _BEGIN_TRANS2_CHECK, _ENABLE) | 
        DRF_DEF(_PGRAPH, _DEBUG_9, _END_TRANSITION_CHECK, _ENABLE);

    m_DebugRegister[10] = 0;

    //
    // Load DEBUG registers.
    //
    REG_WR32(RegisterBase, NV_PGRAPH_DEBUG_0, m_DebugRegister[0]);
    REG_WR32(RegisterBase, NV_PGRAPH_DEBUG_1, m_DebugRegister[1]);
    REG_WR32(RegisterBase, NV_PGRAPH_DEBUG_2, m_DebugRegister[2]);
    REG_WR32(RegisterBase, NV_PGRAPH_DEBUG_3, m_DebugRegister[3]);
    REG_WR32(RegisterBase, NV_PGRAPH_DEBUG_4, m_DebugRegister[4]);
    REG_WR32(RegisterBase, NV_PGRAPH_DEBUG_5, m_DebugRegister[5]);
    REG_WR32(RegisterBase, NV_PGRAPH_DEBUG_6, m_DebugRegister[6]);
    REG_WR32(RegisterBase, NV_PGRAPH_DEBUG_7, m_DebugRegister[7]);
    REG_WR32(RegisterBase, NV_PGRAPH_DEBUG_8, m_DebugRegister[8]);
    REG_WR32(RegisterBase, NV_PGRAPH_DEBUG_9, m_DebugRegister[9]);

    REG_WR32(RegisterBase, NV_PGRAPH_DEBUG_10, m_DebugRegister[10]);


    //
    // Load context buffer base address.
    //
    REG_WR_DRF_NUM(RegisterBase, _PGRAPH, _CHANNEL_CTX_TABLE, _INST, m_HalInfo.GrCtxTableBase);

    //
    // PGRAPH_TILE values will be reset after a PGRAPH_DEBUG_0_STATE_RESET.
    // Unfortunately, it may have had values we wanted (e.g. from stateFb),
    // so after a DEBUG_0_STATE_RESET, restore the PGRAPH values from PFB.
    //

    HalGrIdle();

    for (INT i = 0; i < NV_PFB_TILE__SIZE_1; i++)
    {
        misc = REG_RD32(RegisterBase, NV_PFB_TLIMIT(i));
        REG_WR32(RegisterBase, NV_PGRAPH_TLIMIT(i), misc);
        RDI_REG_WR32(RegisterBase, NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                     NV_IGRAPH_TC_MINTFCDEBUG_TLIMIT_ADDRESS(i),
                     misc);

        misc = REG_RD32(RegisterBase, NV_PFB_TSIZE(i));
        REG_WR32(RegisterBase, NV_PGRAPH_TSIZE(i), misc);
        RDI_REG_WR32(RegisterBase, NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                     NV_IGRAPH_TC_MINTFCDEBUG_TPITCH_ADDRESS(i),
                     misc);

        misc = REG_RD32(RegisterBase, NV_PFB_TILE(i));
        REG_WR32(RegisterBase, NV_PGRAPH_TILE(i), misc);
        RDI_REG_WR32(RegisterBase, NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                     NV_IGRAPH_TC_MINTFCDEBUG_TILE_ADDRESS(i),
                     misc);

        // 
        // These registers need to agree
        //
        ASSERT(REG_RD32(RegisterBase, NV_PFB_TILE(i)) == REG_RD32(RegisterBase, NV_PGRAPH_TILE(i)));
        ASSERT(REG_RD32(RegisterBase, NV_PFB_TLIMIT(i)) == REG_RD32(RegisterBase, NV_PGRAPH_TLIMIT(i)));
        ASSERT(REG_RD32(RegisterBase, NV_PFB_TSIZE(i)) == REG_RD32(RegisterBase, NV_PGRAPH_TSIZE(i)));
    }

    //
    // Restore zcompression table from fb engine.
    //
    for (i = 0; i < NV_PFB_ZCOMP__SIZE_1; i++)
    {
        misc = REG_RD32(RegisterBase, NV_PFB_ZCOMP(i));
        REG_WR32(RegisterBase, NV_PGRAPH_ZCOMP(i), misc);
        RDI_REG_WR32(RegisterBase, NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                     NV_IGRAPH_TC_MINTFCDEBUG_ZCOMP_ADDRESS(i),
                     misc);
    }
    misc = REG_RD32(RegisterBase, NV_PFB_ZCOMP_OFFSET);
    REG_WR32(RegisterBase, NV_PGRAPH_ZCOMP_OFFSET, misc);
    RDI_REG_WR32(RegisterBase, NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                 NV_IGRAPH_TC_MINTFCDEBUG_ZOFFSET_ADDRESS,
                 misc);

    //
    // Restore framebuffer config registers from fb engine.
    //
    misc = REG_RD32(RegisterBase, NV_PFB_CFG0);
    REG_WR32(RegisterBase, NV_PGRAPH_FBCFG0, misc);
    RDI_REG_WR32(RegisterBase, NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                 NV_IGRAPH_TC_MINTFCDEBUG_CFG0_ADDRESS,
                 misc);

    misc = REG_RD32(RegisterBase, NV_PFB_CFG1);
    REG_WR32(RegisterBase, NV_PGRAPH_FBCFG1, misc);
    RDI_REG_WR32(RegisterBase, NV_IGRAPH_TC_MINTFCDEBUG_SELECT,
                 NV_IGRAPH_TC_MINTFCDEBUG_CFG1_ADDRESS,
                 misc);

    REG_WR32(RegisterBase, NV_PGRAPH_CTX_SWITCH1,  0x00000000);
    REG_WR32(RegisterBase, NV_PGRAPH_CTX_SWITCH2,  0x00000000);
    REG_WR32(RegisterBase, NV_PGRAPH_CTX_SWITCH3,  0x00000000);
    REG_WR32(RegisterBase, NV_PGRAPH_CTX_SWITCH4,  0x00000000);
    REG_WR32(RegisterBase, NV_PGRAPH_CTX_CONTROL,
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _MINIMUM_TIME, _33US)
             | DRF_DEF(_PGRAPH, _CTX_CONTROL, _TIME,         _EXPIRED)
             | DRF_DEF(_PGRAPH, _CTX_CONTROL, _CHID,         _INVALID)
             | DRF_DEF(_PGRAPH, _CTX_CONTROL, _SWITCHING,    _IDLE)
             | DRF_DEF(_PGRAPH, _CTX_CONTROL, _DEVICE,       _ENABLED));
    REG_WR_DRF_DEF(RegisterBase, _PGRAPH, _FFINTFC_ST2, _CHID_STATUS, _VALID);

    //
    // Reload current graphics context.
    //
    HalGrLoadChannelContext(m_HalInfo.GrChID);

}



VOID
CMiniport::HalGrIdle()
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;
    ULONG pmc;

    while (REG_RD32(RegisterBase, NV_PGRAPH_STATUS))
    {
        pmc = REG_RD32(RegisterBase, NV_PMC_INTR_0);
        if (pmc & DRF_DEF(_PMC, _INTR_0, _PGRAPH, _PENDING))
        {
            ServiceGrInterrupt();
        }
        if (pmc & DRF_DEF(_PMC, _INTR_0, _PCRTC, _PENDING))
        {
            VBlank();
        }
    }

}




VOID
CMiniport::HalGrLoadChannelContext(
    ULONG ChID
    )
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;
    ULONG misc;

    //
    // Service any outstanding graphics exceptions.
    //
    
    if (REG_RD32(RegisterBase, NV_PGRAPH_INTR))
    {
        ServiceGrInterrupt();
    }

    //
    // Save current graphics interface state.
    //
    
    GR_SAVE_STATE(RegisterBase, misc);

    HalGrIdle();

    //
    // Unload current context.
    //
    
    if (m_HalInfo.GrChID != ChID)
    {
        HalGrUnloadChannelContext(m_HalInfo.GrChID);
    }

    //
    // If the incoming channel is our "invalid" channel, then
    // invalidate and we're done.
    //

    m_HalInfo.GrChID = ChID;
    if (m_HalInfo.GrChID == NUM_FIFOS_NV20)
    {
        REG_WR32(RegisterBase, NV_PGRAPH_CTX_CONTROL,
                 DRF_DEF(_PGRAPH, _CTX_CONTROL, _MINIMUM_TIME, _33US) |
                 DRF_DEF(_PGRAPH, _CTX_CONTROL, _TIME, _NOT_EXPIRED) |
                 DRF_DEF(_PGRAPH, _CTX_CONTROL, _CHID, _INVALID) |
                 DRF_DEF(_PGRAPH, _CTX_CONTROL, _SWITCHING, _IDLE) |
                 DRF_DEF(_PGRAPH, _CTX_CONTROL, _DEVICE, _ENABLED));
        REG_WR_DRF_DEF(RegisterBase, _PGRAPH, _FFINTFC_ST2, _CHID_STATUS, _VALID);

        //
        // Restore graphics interface state.
        //
        
        misc |= DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _ENABLED);
        GR_RESTORE_STATE(RegisterBase, misc); // FIFO reenabled in here

        return;
    }

    //
    // If we have 3D state in this channel, then we need to
    // workaround a problem with FD state (see magnus' chsw3.c
    // diag in //hw/nv20/diag/tests/src for more info).
    //
    
    if (m_HalInfo.GrCurrentObjects3d[ChID] != 0)
    {
        ULONG i;

        // 
        // Reset IDX/VTX/CAS state.
        //

        REG_WR32(RegisterBase, NV_PGRAPH_DEBUG_0,
                 DRF_DEF(_PGRAPH, _DEBUG_0, _IDX_STATE, _RESET) |
                 DRF_DEF(_PGRAPH, _DEBUG_0, _VTX_STATE, _RESET) |
                 DRF_DEF(_PGRAPH, _DEBUG_0, _CAS_STATE, _RESET));
        i = REG_RD32(RegisterBase, NV_PGRAPH_DEBUG_0);
        REG_WR32(RegisterBase, NV_PGRAPH_DEBUG_0, 0x0);
        i = REG_RD32(RegisterBase, NV_PGRAPH_DEBUG_0);

        //
        // Clear FD mode by writing 0 to all FD registers.
        //

        REG_WR32(RegisterBase, NV_PGRAPH_RDI_INDEX,
                 DRF_NUM(_PGRAPH, _RDI_INDEX, _SELECT, RDI_RAMSEL_FD_CTRL) |
                 DRF_NUM(_PGRAPH, _RDI_INDEX, _ADDRESS, 0));
        for (i = 0; i < 15; i++)
            REG_WR32(RegisterBase, NV_PGRAPH_RDI_DATA, 0);
    }

    //
    // Invalidate tcache.
    //
    
    FLD_WR_DRF_DEF(RegisterBase, _PGRAPH, _DEBUG_1, _CACHE, _INVALIDATE);

    REG_WR_DRF_NUM(RegisterBase, _PGRAPH, _CTX_USER, _CHID, ChID);

    //
    // Load new context.
    //
    
    REG_WR_DRF_NUM(RegisterBase, _PGRAPH, _CHANNEL_CTX_POINTER, _INST, m_HalInfo.GrCtxTable[ChID]);
    REG_WR_DRF_DEF(RegisterBase, _PGRAPH, _CHANNEL_CTX_TRIGGER, _READ_IN, _ACTIVATE);

    HalGrIdle();

    //
    // Update hardware with new chid.
    //
    
    FLD_WR_DRF_NUM(RegisterBase, _PGRAPH, _CTX_USER, _CHID, ChID);
    REG_WR32(RegisterBase, NV_PGRAPH_CTX_CONTROL,
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _MINIMUM_TIME, _33US) |
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _TIME, _NOT_EXPIRED) |
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _CHID, _VALID) |
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _SWITCHING, _IDLE) |
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _DEVICE, _ENABLED));

    REG_WR32(RegisterBase, NV_PGRAPH_FFINTFC_ST2, REG_RD32(RegisterBase, NV_PGRAPH_FFINTFC_ST2) & 0xCFFFFFFF);
}



VOID
CMiniport::HalGrUnloadChannelContext(
    ULONG ChID
    )
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;

    //
    // No need to unload an invalid channel
    //
    
    if (ChID == NUM_FIFOS_NV20)
        return;

    //
    // Unload context.
    //
    
    REG_WR_DRF_NUM(RegisterBase, _PGRAPH, _CHANNEL_CTX_POINTER, _INST, m_HalInfo.GrCtxTable[ChID]);
    REG_WR_DRF_DEF(RegisterBase, _PGRAPH, _CHANNEL_CTX_TRIGGER, _WRITE_OUT, _ACTIVATE);

    HalGrIdle();

    //
    // Invalidate hw's channel ID.
    //
    
    REG_WR32(RegisterBase, NV_PGRAPH_CTX_CONTROL,
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _MINIMUM_TIME, _33US) |
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _TIME, _EXPIRED) |
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _CHID, _INVALID) |
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _SWITCHING, _IDLE) |
             DRF_DEF(_PGRAPH, _CTX_CONTROL, _DEVICE, _ENABLED));

}



VOID
CMiniport::HalFifoControlLoad()
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;

    //
    // Update the dma fetch numbers using reasonable defaults.
    // We don't need to worry about the endianness bit here
    // because we're going to swap this channel out asap
    //

    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_DMA_FETCH,
             DRF_DEF(_PFIFO, _CACHE1_DMA_FETCH, _TRIG, _128_BYTES) |
             DRF_DEF(_PFIFO, _CACHE1_DMA_FETCH, _SIZE, _32_BYTES) |
             DRF_DEF(_PFIFO, _CACHE1_DMA_FETCH, _MAX_REQS, _15));

    //
    // Enable default timeslicing
    //
    REG_WR32(RegisterBase, NV_PFIFO_DMA_TIMESLICE,
             DRF_DEF(_PFIFO, _DMA_TIMESLICE, _TIMEOUT, _ENABLED)
             | DRF_DEF(_PFIFO, _DMA_TIMESLICE, _SELECT, _128K));

    //
    // Set retry delay to match common hardware latency.
    //
    
    REG_WR_DRF_NUM(RegisterBase, _PFIFO, _DELAY_0, _WAIT_RETRY, m_HalInfo.FifoRetryCount);
    
    //
    // Disable cache reassignment.
    //
    
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHES, _REASSIGN, _DISABLED);
    
    //
    // Disable pusher and puller access to cache0.
    //
    
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE0_PUSH0, _ACCESS, _DISABLED);
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE0_PULL0, _ACCESS, _DISABLED);
    
    //
    // Disable pusher and puller access to cache1.
    //
    
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE1_PUSH0, _ACCESS, _DISABLED);
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE1_PULL0, _ACCESS, _DISABLED);

    //
    // Disable the pusher, if active
    //
    
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE1_DMA_PUSH, _ACCESS, _DISABLED);

    //
    // Force a context switch to PRAMIN.
    //
    
    HalFifoContextSwitch((NUM_FIFOS_NV20 - 1));

    //
    // Zero FIFO get/put.
    //
    
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_PUT, 0);
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_GET, 0);

    //
    // Enable pusher and puller access to cache1.
    //
    
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE1_PULL0, _ACCESS, _ENABLED);
    
    //
    // Delay for slow CACHE1_PULL.
    //
    
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE1_PUSH0, _ACCESS, _ENABLED);
    
    //
    // Enable cache reassignment.
    //
    
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHES, _REASSIGN, _ENABLED);

    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHES, _REASSIGN, _DISABLED);
}



VOID
CMiniport::HalFifoContextSwitch(
    ULONG ChID
    )
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;
    ULONG FifoReassign, FifoPush, FifoPull;
    ULONG CurrentChID;
    ULONG PushPending;

    ULONG CtxtPtr = m_HalInfo.FifoContextAddr1;

    //
    // Save CACHE1 state.
    //
    
    FifoReassign = REG_RD32(RegisterBase, NV_PFIFO_CACHES);
    FifoPush = REG_RD32(RegisterBase, NV_PFIFO_CACHE1_PUSH0);
    FifoPull = REG_RD32(RegisterBase, NV_PFIFO_CACHE1_PULL0);
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHES, _REASSIGN, _DISABLED);
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE1_PUSH0, _ACCESS, _DISABLED);
    REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE1_PULL0, _ACCESS, _DISABLED);

    //
    // Make sure our local (rm) contexts are up to date for the outgoing channel
    //
    
    CurrentChID = REG_RD_DRF(RegisterBase, _PFIFO, _CACHE1_PUSH1, _CHID);

    REG_WR32(RegisterBase, (ULONG)(CtxtPtr + (CurrentChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_DMA_PUT)), REG_RD32(RegisterBase, NV_PFIFO_CACHE1_DMA_PUT));
    REG_WR32(RegisterBase, (ULONG)(CtxtPtr + (CurrentChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_DMA_GET)), REG_RD32(RegisterBase, NV_PFIFO_CACHE1_DMA_GET));
    REG_WR32(RegisterBase, (ULONG)(CtxtPtr + (CurrentChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_REF_CNT)), REG_RD32(RegisterBase, NV_PFIFO_CACHE1_REF));
    REG_WR32(RegisterBase, (ULONG)(CtxtPtr + (CurrentChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_DMA_INST)), REG_RD32(RegisterBase, NV_PFIFO_CACHE1_DMA_INSTANCE));
    REG_WR32(RegisterBase, (ULONG)(CtxtPtr + (CurrentChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_DMA_METHOD)), REG_RD32(RegisterBase, NV_PFIFO_CACHE1_DMA_STATE));
    REG_WR32(RegisterBase, (ULONG)(CtxtPtr + (CurrentChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_DMA_FETCH_TRIG)), REG_RD32(RegisterBase, NV_PFIFO_CACHE1_DMA_FETCH));
    REG_WR32(RegisterBase, (ULONG)(CtxtPtr + (CurrentChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_ENGINE_SUB_0)), REG_RD32(RegisterBase, NV_PFIFO_CACHE1_ENGINE));
    REG_WR32(RegisterBase, (ULONG)(CtxtPtr + (CurrentChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_PULL1_ENGINE)), REG_RD32(RegisterBase, NV_PFIFO_CACHE1_PULL1));
    REG_WR32(RegisterBase, (ULONG)(CtxtPtr + (CurrentChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_ACQUIRE_VALUE)), REG_RD32(RegisterBase, NV_PFIFO_CACHE1_ACQUIRE_2));
    REG_WR32(RegisterBase, (ULONG)(CtxtPtr + (CurrentChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_ACQUIRE_TIMESTAMP)), REG_RD32(RegisterBase, NV_PFIFO_CACHE1_ACQUIRE_1));
    REG_WR32(RegisterBase, (ULONG)(CtxtPtr + (CurrentChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_ACQUIRE_TIMEOUT)), REG_RD32(RegisterBase, NV_PFIFO_CACHE1_ACQUIRE_0));
    REG_WR32(RegisterBase, (ULONG)(CtxtPtr + (CurrentChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_SEMAPHORE_CTXDMA)), REG_RD32(RegisterBase, NV_PFIFO_CACHE1_SEMAPHORE));
    REG_WR32(RegisterBase, (ULONG)(CtxtPtr + (CurrentChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_DMA_SUBROUTINE_STATE)), REG_RD32(RegisterBase, NV_PFIFO_CACHE1_DMA_SUBROUTINE));

    //
    // If this is a push channel and put != get, make sure to set the push pending flag
    //
    
    if (REG_RD32(RegisterBase, NV_PFIFO_CACHE1_PUSH1) & DRF_DEF(_PFIFO, _CACHE1_PUSH1, _MODE, _DMA))
    {
        PushPending = REG_RD32(RegisterBase, NV_PFIFO_DMA) & ~(1 << CurrentChID);
        if (REG_RD32(RegisterBase, NV_PFIFO_CACHE1_DMA_PUT) != REG_RD32(RegisterBase, NV_PFIFO_CACHE1_DMA_GET))
        {
            PushPending |= (1 << CurrentChID);
        }
        REG_WR32(RegisterBase, NV_PFIFO_DMA, PushPending);
    }

    //
    // Update the channel and if it's DMA, set the mode
    //
    
    REG_WR_DRF_NUM(RegisterBase, _PFIFO, _CACHE1_PUSH1, _CHID, ChID);
    if ((m_HalInfo.FifoMode & (1 << ChID)) && (ChID != (NUM_FIFOS_NV20 - 1)))
    {
        FLD_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE1_PUSH1, _MODE, _DMA);
    }

    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_DMA_PUT, REG_RD32(RegisterBase, (ULONG)(CtxtPtr + (ChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_DMA_PUT))));
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_DMA_GET, REG_RD32(RegisterBase, (ULONG)(CtxtPtr + (ChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_DMA_GET))));
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_REF,REG_RD32(RegisterBase, (ULONG)(CtxtPtr + (ChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_REF_CNT))));
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_DMA_INSTANCE, REG_RD32(RegisterBase, (ULONG)(CtxtPtr + (ChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_DMA_INST))));
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_DMA_STATE, REG_RD32(RegisterBase, (ULONG)(CtxtPtr + (ChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_DMA_METHOD))));
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_DMA_FETCH, REG_RD32(RegisterBase, (ULONG)(CtxtPtr + (ChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_DMA_FETCH_TRIG))));
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_ENGINE, REG_RD32(RegisterBase, (ULONG)(CtxtPtr + (ChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_ENGINE_SUB_0))));
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_PULL1, REG_RD32(RegisterBase, (ULONG)(CtxtPtr + (ChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_PULL1_ENGINE))));
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_ACQUIRE_2, REG_RD32(RegisterBase, (ULONG)(CtxtPtr + (ChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_ACQUIRE_VALUE))));
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_ACQUIRE_1, REG_RD32(RegisterBase, (ULONG)(CtxtPtr + (ChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_ACQUIRE_TIMESTAMP))));
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_ACQUIRE_0, REG_RD32(RegisterBase, (ULONG)(CtxtPtr + (ChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_ACQUIRE_TIMEOUT))));
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_SEMAPHORE, REG_RD32(RegisterBase, (ULONG)(CtxtPtr + (ChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_SEMAPHORE_CTXDMA))));
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_DMA_SUBROUTINE, REG_RD32(RegisterBase, (ULONG)(CtxtPtr + (ChID * FC1_SIZE_NV20) + SF_OFFSET(NV_RAMFC_DMA_SUBROUTINE_STATE))));

    //
    // If DMA, enable the DMA Pusher
    //
    
    if ((m_HalInfo.FifoMode & (1 << ChID)) && (ChID != (NUM_FIFOS_NV20 - 1)))
    {
        REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE1_DMA_PUSH, _ACCESS, _ENABLED);
    }

    //
    // In case there are dma pusher channels still pending, give this
    // new channel enough time to do something useful before it gets
    // yanked back out again.
    //
    
    REG_WR32(RegisterBase, NV_PFIFO_TIMESLICE, 0x1fffff);

    //
    // Restore CACHE1 state.
    //
    
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_PULL0, FifoPull);
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_PUSH0, FifoPush);
    REG_WR32(RegisterBase, NV_PFIFO_CACHES, FifoReassign);
}



VOID
CMiniport::HalMpControlInit()
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;
    ULONG hostConfig = REG_RD32(RegisterBase, NV_PME_HOST_CONFIG);
    
    if ((hostConfig & DRF_DEF(_PME, _HOST_CONFIG, _SLAVE_DETECTED, _TRUE)) &&
        (hostConfig & DRF_DEF(_PME, _HOST_CONFIG, _SLAVE_NOTDETECTED, _FALSE)) )
    {
        m_GenInfo.MpVIPSlavePresent = 1;
    }
    else
    {
        m_GenInfo.MpVIPSlavePresent = 0;
    }

    //
    // Initialize media engine's config.  Make sure all the media engine interrupts are disabled
    //

    REG_WR32(RegisterBase, NV_PME_INTR_EN_0, 0);    

    FLD_WR_DRF_NUM(RegisterBase, _PME, _HOST_CONFIG, _FIFOMAXTX, 32);
    FLD_WR_DRF_DEF(RegisterBase, _PME, _HOST_CONFIG, _CLOCK_SELECT, _PCI);
    FLD_WR_DRF_NUM(RegisterBase, _PME, _HOST_CONFIG, _TIMEOUT, 7);    

    //
    // Initialize media engine's various DMA engines
    //

    REG_WR32(RegisterBase, NV_PME_ANC_ME_STATE, 0);
    REG_WR32(RegisterBase, NV_PME_ANC_SU_STATE, 0);
    REG_WR32(RegisterBase, NV_PME_ANC_RM_STATE, 0);
    REG_WR32(RegisterBase, NV_PME_TASKA_ME_STATE, 0);
    REG_WR32(RegisterBase, NV_PME_TASKA_SU_STATE, 0);
    REG_WR32(RegisterBase, NV_PME_TASKA_RM_STATE, 0);
    REG_WR32(RegisterBase, NV_PME_TASKB_ME_STATE, 0);
    REG_WR32(RegisterBase, NV_PME_TASKB_SU_STATE, 0);
    REG_WR32(RegisterBase, NV_PME_TASKB_RM_STATE, 0);
    REG_WR32(RegisterBase, NV_PME_FOUT_ME_STATE, 0);
    REG_WR32(RegisterBase, NV_PME_FOUT_SU_STATE, 0);
    REG_WR32(RegisterBase, NV_PME_FOUT_RM_STATE, 0);
    REG_WR32(RegisterBase, NV_PME_FIN_ME_STATE, 0);
    REG_WR32(RegisterBase, NV_PME_FIN_SU_STATE, 0);
    REG_WR32(RegisterBase, NV_PME_FIN_RM_STATE, 0);

    //
    // Clear 656_CONFIG
    //

    REG_WR32(RegisterBase, NV_PME_656_CONFIG, 0);

    //
    // Initialize FOUT_ADDR and FIN_ADDR to something
    //

    FLD_WR_DRF_NUM(RegisterBase, _PME, _FOUT_ADDR, _FIFO, 4);   
    FLD_WR_DRF_NUM(RegisterBase, _PME, _FOUT_ADDR, _DEVICE, 0);

    FLD_WR_DRF_NUM(RegisterBase, _PME, _FIN_ADDR, _FIFO, 5);    
    FLD_WR_DRF_NUM(RegisterBase, _PME, _FIN_ADDR, _DEVICE, 0);
}



VOID
CMiniport::HalDacUnload()
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;

    //
    // Disable the cursor and the vertical blank interrupt (on head 0)
    //

    DAC_REG_WR_DRF_DEF(RegisterBase, _PCRTC, _INTR_0, _VBLANK, _RESET);
    DAC_REG_WR_DRF_DEF(RegisterBase, _PCRTC, _INTR_EN_0, _VBLANK, _DISABLED);
}


VOID
CMiniport::HalDacLoad()
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;

    //
    // Enable the vertical blank interrupt on primary device (on head 0)
    //

    DAC_REG_WR_DRF_DEF(RegisterBase, _PCRTC, _INTR_0, _VBLANK, _RESET);
    DAC_REG_WR_DRF_DEF(RegisterBase, _PCRTC, _INTR_EN_0, _VBLANK, _ENABLED);

}



VOID
CMiniport::HalDacProgramMClk()
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;

    ULONG mNew;
    ULONG nNew;
    ULONG pNew;
    ULONG mOld;
    ULONG nOld;
    ULONG pOld;
    ULONG coeff;
    ULONG oldMClk;
    ULONG oldCyclesPerRefresh;
    ULONG oldPeriod;
    ULONG newCyclesPerRefresh;
    ULONG Loops;
    ULONG IsPllLocked;
    ULONG Emrs;
    ULONG NewCoeff;

    mNew = m_DacInfo.MPllM;
    nNew = m_DacInfo.MPllN;
    pNew = m_DacInfo.MPllP;

    coeff = REG_RD32(RegisterBase,  NV_PRAMDAC_MPLL_COEFF );

    NewCoeff = DRF_NUM( _PRAMDAC, _MPLL_COEFF, _MDIV, mNew) |
               DRF_NUM( _PRAMDAC, _MPLL_COEFF, _NDIV, nNew) |
               DRF_NUM( _PRAMDAC, _MPLL_COEFF, _PDIV, pNew);

    if (coeff == NewCoeff)
    {
        //
        // WORKAROUND: Don't set the clock if its already set to the right value.  In
        // other words, if its a "quick reboot" the clock value is already set from the last boot.
        // This is done to avoid running into a bug which causes incorrect value for NV_PFB_TIMING2
        // register at the end of this function due to loss of precision if called more than once.
        // NVidia is working on fixing that problem.  This workaround, however, is actually good
        // since there is no need to set the clock again if its already correctly set.
        //
        return;
    }

    WARNING("Setting MClk to %d", m_DacInfo.MClk / 2000000);

    mOld = DRF_VAL( _PRAMDAC, _MPLL_COEFF, _MDIV, coeff);
    nOld = DRF_VAL( _PRAMDAC, _MPLL_COEFF, _NDIV, coeff);
    pOld = DRF_VAL( _PRAMDAC, _MPLL_COEFF, _PDIV, coeff);

    if ( pNew < pOld )
    {
        // 
        // Write M and N first
        //

        REG_WR32(RegisterBase, NV_PRAMDAC_MPLL_COEFF,
                 DRF_NUM( _PRAMDAC, _MPLL_COEFF, _MDIV, mNew) |
                 DRF_NUM( _PRAMDAC, _MPLL_COEFF, _NDIV, nNew) |
                 DRF_NUM( _PRAMDAC, _MPLL_COEFF, _PDIV, pOld));

        // 
        // Wait until M PLL is locked
        // 

        IsPllLocked = FALSE;
        for ( Loops = 100; Loops; --Loops )
        {
            if (REG_RD_DRF(RegisterBase, _PRAMDAC, _PLL_TEST_COUNTER, _MPLL_LOCK) ==
               NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_LOCKED)
            {
                if (REG_RD_DRF(RegisterBase, _PRAMDAC, _PLL_TEST_COUNTER, _MPLL_LOCK) ==
                    NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_LOCKED)
                {
                    IsPllLocked = TRUE;
                    break;
                }
            }

            KeStallExecutionProcessor(1 * 1000);   // 1ms
        }

        if ( !IsPllLocked )
        {
            WARNING("MP: M PLL not locked\n");
        }
    }

    // Now we can write the final value
    REG_WR32(RegisterBase,
             NV_PRAMDAC_MPLL_COEFF,
             DRF_NUM( _PRAMDAC, _MPLL_COEFF, _MDIV, mNew) |
             DRF_NUM( _PRAMDAC, _MPLL_COEFF, _NDIV, nNew) |
             DRF_NUM( _PRAMDAC, _MPLL_COEFF, _PDIV, pNew));

    //
    // Wait for M PLL to lock.
    // !!! PERF: see if we actually need to wait this long.  This seems way too long
    //
    
    KeStallExecutionProcessor(64 * 1000);   // 64ms

    //
    // Reset EMRS.
    //
    
    Emrs = REG_RD32(RegisterBase, NV_PFB_EMRS);
    Emrs |= DRF_DEF(_PFB, _EMRS, _BA0, _1);
    REG_WR32(RegisterBase, NV_PFB_EMRS, Emrs);

    // 
    // Reset (resync) the frame buffer DRAM DLL.
    //

    FLD_WR_DRF_DEF(RegisterBase, _PFB, _MRS, _A8, _1);

    oldMClk  = (nOld * m_GenInfo.CrystalFreq / (1 << pOld) / mOld);
    oldCyclesPerRefresh = REG_RD_DRF(RegisterBase,  _PFB, _TIMING2, _REFRESH ) * 32;
    oldPeriod = oldCyclesPerRefresh * 1024 / (oldMClk / 1000);

    newCyclesPerRefresh = (m_DacInfo.MClk / 1000) * oldPeriod / 1024;

    // 
    // Store the new value
    //

    FLD_WR_DRF_NUM(RegisterBase, _PFB, _TIMING2, _REFRESH, newCyclesPerRefresh / 32);
}


VOID
CMiniport::HalDacProgramNVClk()
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;
    ULONG mNew;
    ULONG nNew;
    ULONG pNew;
    ULONG mOld;
    ULONG nOld;
    ULONG pOld;
    ULONG coeff;
    ULONG Loops;
    ULONG IsPllLocked;

    mNew = m_DacInfo.NVPllM;
    nNew = m_DacInfo.NVPllN;
    pNew = m_DacInfo.NVPllP;

    //
    // Retrieve original coefficients
    //
    
    coeff = REG_RD32(RegisterBase,  NV_PRAMDAC_NVPLL_COEFF );
    mOld = DRF_VAL( _PRAMDAC, _NVPLL_COEFF, _MDIV, coeff);
    nOld = DRF_VAL( _PRAMDAC, _NVPLL_COEFF, _NDIV, coeff);
    pOld = DRF_VAL( _PRAMDAC, _NVPLL_COEFF, _PDIV, coeff);

    if (pNew < pOld)
    {
        //
        // Write M and N first
        //

        REG_WR32(RegisterBase,
                 NV_PRAMDAC_NVPLL_COEFF,
                 DRF_NUM( _PRAMDAC, _NVPLL_COEFF, _MDIV, mNew) |
                 DRF_NUM( _PRAMDAC, _NVPLL_COEFF, _NDIV, nNew) |
                 DRF_NUM( _PRAMDAC, _NVPLL_COEFF, _PDIV, pOld)
                );

        //
        // Wait until NV PLL is locked
        //
        
        IsPllLocked = FALSE;
        for (Loops = 100; Loops; --Loops)
        {
            if (REG_RD_DRF(RegisterBase, _PRAMDAC, _PLL_TEST_COUNTER, _NVPLL_LOCK) ==
                           NV_PRAMDAC_PLL_TEST_COUNTER_NVPLL_LOCKED)
            {
                if (REG_RD_DRF(RegisterBase, _PRAMDAC, _PLL_TEST_COUNTER, _NVPLL_LOCK) ==
                               NV_PRAMDAC_PLL_TEST_COUNTER_NVPLL_LOCKED)
                {
                    IsPllLocked = TRUE;
                    break;
                }
            }

            KeStallExecutionProcessor(1 * 1000);   // 1ms
        }

        if (!IsPllLocked)
        {
            WARNING("MP: NV PLL not locked\n");
        }
    }

    //
    // Now we can write the final value
    //
    
    REG_WR32(RegisterBase,
             NV_PRAMDAC_NVPLL_COEFF,
             DRF_NUM( _PRAMDAC, _NVPLL_COEFF, _MDIV, mNew) |
             DRF_NUM( _PRAMDAC, _NVPLL_COEFF, _NDIV, nNew) |
             DRF_NUM( _PRAMDAC, _NVPLL_COEFF, _PDIV, pNew)
            );


    //
    // Update NV_PBUS_DEBUG_3 to control data throttling in AGP 4X mode
    //

    ULONG Setting = NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_66MHZ - 1;
    switch (m_DacInfo.NVClk)
    {
        case 133000000: Setting = NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_133MHZ; break;
        case 126000000: Setting = NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_126MHZ; break;
        case 120000000: Setting = NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_120MHZ; break;
        case 113000000: Setting = NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_113MHZ; break;
        case 106000000: Setting = NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_106MHZ; break;
        case 100000000: Setting = NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_100MHZ; break;
        case 94000000: Setting = NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_94MHZ; break;
        case 87000000: Setting = NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_87MHZ; break;
        case 80000000: Setting = NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_80MHZ; break;
        case 73000000: Setting = NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_73MHZ; break;
        case 66000000: Setting = NV_PBUS_DEBUG_3_AGP_4X_NVCLK_ABOVE_66MHZ; break;
    }

    FLD_WR_DRF_NUM(RegisterBase, _PBUS, _DEBUG_3, _AGP_4X_NVCLK, Setting);
}



VOID
CMiniport::HalDacProgramPClk()
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;
    ULONG mNew;
    ULONG nNew;
    ULONG pNew;
    ULONG mOld;
    ULONG nOld;
    ULONG pOld;
    ULONG coeff;
    ULONG Loops;
    ULONG IsPllLocked;

    mNew = m_DacInfo.VPllM;
    nNew = m_DacInfo.VPllN;
    pNew = m_DacInfo.VPllP;


    //
    // Retrieve original coefficients
    //
    
    coeff = REG_RD32(RegisterBase,  NV_PRAMDAC_VPLL_COEFF );

    mOld = DRF_VAL( _PRAMDAC, _VPLL_COEFF, _MDIV, coeff);
    nOld = DRF_VAL( _PRAMDAC, _VPLL_COEFF, _NDIV, coeff);
    pOld = DRF_VAL( _PRAMDAC, _VPLL_COEFF, _PDIV, coeff);

    if (pNew < pOld)
    {
        //
        // Write M and N first (head 0)
        //
        REG_WR32(RegisterBase,
                 NV_PRAMDAC_VPLL_COEFF,
                 DRF_NUM( _PRAMDAC, _VPLL_COEFF, _MDIV, mNew) |
                 DRF_NUM( _PRAMDAC, _VPLL_COEFF, _NDIV, nNew) |
                 DRF_NUM( _PRAMDAC, _VPLL_COEFF, _PDIV, pOld));

        //
        // Wait until V PLL is locked
        //
        IsPllLocked = FALSE;
        for (Loops = 100; Loops; --Loops)
        {
            if (REG_RD_DRF(RegisterBase, _PRAMDAC, _PLL_TEST_COUNTER, _VPLL_LOCK) ==
                           NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_LOCKED)
            {
                // Check again, just to make sure.   !!!PERF: Why?
                if (REG_RD_DRF(RegisterBase, _PRAMDAC, _PLL_TEST_COUNTER, _VPLL_LOCK) ==
                               NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_LOCKED)
                {
                    IsPllLocked = TRUE;
                    break;
                }
            }

            KeStallExecutionProcessor(1 * 1000);   // 1ms
        }

        if (!IsPllLocked)
        {
            WARNING("MP: VPLL not locked\n");
        }
    }

    //
    // Now we can write the final value
    //

    REG_WR32(RegisterBase,
             NV_PRAMDAC_VPLL_COEFF,
             DRF_NUM( _PRAMDAC, _VPLL_COEFF, _MDIV, mNew) |
             DRF_NUM( _PRAMDAC, _VPLL_COEFF, _NDIV, nNew) |
             DRF_NUM( _PRAMDAC, _VPLL_COEFF, _PDIV, pNew));

}


void
CMiniport::HalFifoAllocDMA(
    ULONG fetchTrigger,
    ULONG fetchSize,
    ULONG fetchRequests,
    OBJECTINFO* DataContext
    )
{

    BYTE* RegisterBase = (BYTE*)m_RegisterBase;
    ULONG CacheData, InitCtxtPtr, grCtxTable;
    ULONG DefaultFetch;
    ULONG i;
    ULONG ChID = m_HalInfo.FifoChID;

    //
    // Disable cache reassignment.
    //
    
    CacheData = REG_RD32(RegisterBase, NV_PFIFO_CACHES);
    REG_WR_DRF_DEF(RegisterBase, _PFIFO_, CACHES, _REASSIGN, _DISABLED);

    //
    // Clear graphics context buffer.
    //
    
    for (i = 0; i < NV20_GR_CONTEXT_SIZE/4; i++)
        INST_WR32(RegisterBase, m_HalInfo.FifoInstance, i*4, 0);

    grCtxTable = m_HalInfo.GrCtxTableBase;

    ASSERT(INST_RD32(RegisterBase, grCtxTable, ChID * 4) == 0);
    ASSERT(m_HalInfo.GrCtxTable[ChID] == 0);

    INST_WR32(RegisterBase, grCtxTable, ChID * 4, m_HalInfo.FifoInstance);
    m_HalInfo.GrCtxTable[ChID] = m_HalInfo.FifoInstance;

    //
    // Initialize the allocated FIFO context.
    //
    
    InitCtxtPtr = m_HalInfo.FifoContextAddr1 + (ChID * FC1_SIZE_NV20);
    for (i = 0 ; i < FC1_SIZE_NV20/4; i++ )
    {
        REG_WR32(RegisterBase, InitCtxtPtr+(i*4), 0);
    }

    //
    // Insert the instance (note that the rest of the context can start as zero)
    //
    
    REG_WR32(RegisterBase, (ULONG)(InitCtxtPtr + SF_OFFSET(NV_RAMFC_DMA_INST)), DataContext->Instance);

    //
    // NOTE: We only support little endian
    //
    
    DefaultFetch = NV_PFIFO_CACHE1_DMA_FETCH_LITTLE_ENDIAN;

    //
    // Adjust fetch max request (exception to the "all start as zero" rule)
    //

    ULONG dmaFetch;

    //
    // Input trigger value is in bytes.
    // Hardware value must be specified in increments of 8 between
    // 8 - 256 bytes.
    //
    
    if (fetchTrigger < 8)
        fetchTrigger = 8;
    if (fetchTrigger > 256)
        fetchTrigger = 256;
    fetchTrigger = (fetchTrigger / 8) - 1;

    //
    // Input size value is in bytes.
    // Hardware value must be specified in increments of 32
    // between 32 - 256 bytes.
    //
    
    if (fetchSize < 32)
        fetchSize = 32;
    if (fetchSize > 256)
        fetchSize = 256;
    fetchSize = (fetchSize / 32) - 1;

    //
    // Requests must be between 0 and 15.
    //
    
    if (fetchRequests > 15)
        fetchRequests = 15;

    //
    // Put it all together.
    //
    
    dmaFetch = DRF_NUM(_PFIFO, _CACHE1_DMA_FETCH, _TRIG, fetchTrigger) |
        DRF_NUM(_PFIFO, _CACHE1_DMA_FETCH, _SIZE, fetchSize) |
        DRF_NUM(_PFIFO, _CACHE1_DMA_FETCH, _MAX_REQS, fetchRequests);

    DefaultFetch |= dmaFetch;

    REG_WR32(RegisterBase, (ULONG)(InitCtxtPtr + SF_OFFSET(NV_RAMFC_DMA_FETCH_TRIG)), DefaultFetch);

    //
    // Set channel mode to dma
    //
    
    m_HalInfo.FifoMode |= (1 << ChID);
    REG_WR32(RegisterBase, NV_PFIFO_MODE, m_HalInfo.FifoMode);

    //
    // Disable cache1.
    //
    
    REG_WR_DRF_DEF(RegisterBase, _PFIFO_, CACHE1_PUSH0, _ACCESS, _DISABLED);
    REG_WR_DRF_DEF(RegisterBase, _PFIFO_, CACHE1_PULL0, _ACCESS, _DISABLED);
    
    //
    // Set initial cache parameters.
    //
    
    REG_WR_DRF_NUM(RegisterBase, _PFIFO, _CACHE1_PUSH1, _CHID,   ChID);

    if (m_HalInfo.FifoMode & (1 << ChID))
        FLD_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE1_PUSH1, _MODE, _DMA);

    REG_WR_DRF_NUM(RegisterBase, _PFIFO, _CACHE1_DMA_PUT, _OFFSET, 0);
    REG_WR_DRF_NUM(RegisterBase, _PFIFO, _CACHE1_DMA_GET, _OFFSET, 0);

    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_DMA_INSTANCE, DataContext->Instance);

    //
    // Force a reload of the PTE's
    //
    
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_DMA_CTL, 0);

    //
    // Clear all pusher state
    //
    
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_DMA_STATE, 0);
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_ENGINE, 0);

    //
    // Set default watermarks
    //
    
    REG_WR32(RegisterBase, NV_PFIFO_CACHE1_DMA_FETCH, DefaultFetch);

    //
    // Enable cache1.
    //
    
    if (m_HalInfo.FifoMode & (1 << ChID))
        REG_WR_DRF_DEF(RegisterBase, _PFIFO, _CACHE1_DMA_PUSH, _ACCESS, _ENABLED);

    REG_WR_DRF_DEF(RegisterBase, _PFIFO_, CACHE1_PULL0, _ACCESS,   _ENABLED);
    REG_WR_DRF_DEF(RegisterBase, _PFIFO_, CACHE1_PUSH0, _ACCESS,   _ENABLED);
    CacheData = DRF_DEF(_PFIFO_, CACHES, _REASSIGN, _ENABLED);

    //
    // Restore previous cache reassignability.
    //
    
    REG_WR32(RegisterBase, NV_PFIFO_CACHES, CacheData);

    //
    // Mark channel as allocated.
    //
    
    m_HalInfo.FifoInUse |= (1 << ChID);
}



VOID
CMiniport::HalFifoHashAdd(
    ULONG entry,
    ULONG handle,
    ULONG chid,
    ULONG instance,
    ULONG engine
    )
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;

    ASSERT(entry * sizeof(HW_HASHENTRY) < 0x1000);

    ULONG Addr = m_HalInfo.HashTableAddr + (entry * sizeof(HW_HASHENTRY));

    REG_WR32(RegisterBase, Addr, handle);

    REG_WR32(RegisterBase, Addr + 4,
             (instance & SF_MASK(NV_RAMHT_INSTANCE)) |
             (engine << SF_SHIFT(NV_RAMHT_ENGINE)) |
             (chid << SF_SHIFT(NV_RAMHT_CHID)) |
             (NV_RAMHT_STATUS_VALID << SF_SHIFT(NV_RAMHT_STATUS)));

}




VOID
CMiniport::HalGrInitObjectContext(
    ULONG Instance,
    ULONG ClassNum
    )
{

    BYTE* RegisterBase = (BYTE*)m_RegisterBase;
    U032 ctx1, ctx2, ctx3;

    //
    // Default _CTX_SWITCH1 value
    //

    ctx1 = DRF_NUM(_PGRAPH, _CTX_SWITCH1, _GRCLASS, ClassNum)
        | DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_CONFIG, _SRCCOPY_AND)
        | DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_STATUS, _INVALID);

    // 
    // Default _CTX_SWITCH2 value
    //

    ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
        | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _INVALID)
        | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);

    // 
    // Default _CTX_SWITCH3 value
    //

    ctx3 = DRF_DEF(_PGRAPH, _CTX_SWITCH3, _DMA_INSTANCE_0, _INVALID)
        | DRF_DEF(_PGRAPH, _CTX_SWITCH3, _DMA_INSTANCE_1, _INVALID);

    switch (ClassNum)
    {
    case NV20_KELVIN_PRIMITIVE:
        ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
            | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_R5G6B5)
            | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
        break;
    case NV03_MEMORY_TO_MEMORY_FORMAT:
        // Note that this object is always considered patch valid
        ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_STATUS, _VALID);
        break;
    case NV15_IMAGE_BLIT:
        // defaults are fine
        break;
    case NV10_CONTEXT_SURFACES_2D:
        // defaults are fine
        break;

    case NV04_CONTEXT_PATTERN:
        ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _CGA6_M1)
            | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_X16A8Y8)
            | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
        break;

    case NV04_CONTEXT_COLOR_KEY:
        ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
            | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_A16R5G6B5)
            | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
        break;
        
    case NV01_CONTEXT_BETA:
    case NV04_CONTEXT_BETA:
    case NV03_CONTEXT_ROP:
    case NV01_CONTEXT_CLIP_RECTANGLE:
        // defaults are fine
        break;

    case NV01_NULL:
        break;

    default:
        NODEFAULT("Unexpected class\n");

    /*
        case NV01_CONTEXT_BETA:
        case NV01_CONTEXT_CLIP_RECTANGLE:
        case NV01_IMAGE_BLIT:
        case NV03_CONTEXT_ROP:
        case NV04_CONTEXT_SURFACES_2D:
        case NV03_DX3_TEXTURED_TRIANGLE:
        case NV04_CONTEXT_SURFACE_SWIZZLED:
        case NV04_CONTEXT_SURFACES_3D:
        case NV03_CONTEXT_SURFACE_2D_DESTINATION:
        case NV03_CONTEXT_SURFACE_2D_SOURCE:
        case NV03_CONTEXT_SURFACE_3D_COLOR:
        case NV03_CONTEXT_SURFACE_3D_DEPTH:
        case NV04_IMAGE_BLIT:
        case NV04_CONTEXT_BETA:
        case NV10_CONTEXT_SURFACES_2D:
        case NV10_CONTEXT_SURFACES_3D:
        case NV15_CONTEXT_SURFACE_SWIZZLED:
        case NV15_IMAGE_BLIT:
            // defaults are fine
            break;
        case NV20_IMAGE_BLIT:
            ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _CLASS_TYPE, _PERFORMANCE);
            break;
        case NV01_CONTEXT_COLOR_KEY:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_X16A8Y8)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV01_CONTEXT_PATTERN:
        case NV04_CONTEXT_PATTERN:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _CGA6_M1)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_X16A8Y8)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV01_RENDER_SOLID_LIN:
        case NV01_RENDER_SOLID_TRIANGLE:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_X24Y8)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV01_IMAGE_FROM_CPU:
        case NV03_STRETCHED_IMAGE_FROM_CPU:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_Y8)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV1_NULL:
            //
            // The NULL object is really a pseudo-dma object, so it needs a valid
            // limit value and should also have the PAGE_TABLE_PRESENT bit set.
            //
            ctx1 |= SF_DEF(_DMA, _PAGE_TABLE, _PRESENT);
            ctx2 = 0xffffffff;
            //INST_WR32(instance, SF_OFFSET(NV_PRAMIN_CONTEXT_1), 0xFFFFFFFF);
            //FLD_WR_ISF_DEF(instance, _DMA, _PAGE_TABLE, _PRESENT);
            break;
        case NV03_SCALED_IMAGE_FROM_MEMORY:
        case NV04_SCALED_IMAGE_FROM_MEMORY:
        case NV10_SCALED_IMAGE_FROM_MEMORY:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_A1R5G5B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV11_SCALED_IMAGE_FROM_MEMORY:
            ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _CLASS_TYPE, _PERFORMANCE);
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_A1R5G5B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV04_DVD_SUBPICTURE:
        case NV10_DVD_SUBPICTURE:
            //
            // DVD class is patchless, so it is always considered valid.  HW doesn't
            // use this bit, but let's set it anyway to be consistent with other
            // objects
            //
            ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_STATUS, _VALID);
            break;
        case NV03_MEMORY_TO_MEMORY_FORMAT:
            // Note that this object is always considered patch valid
            ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_STATUS, _VALID);
            break;
        case NV04_GDI_RECTANGLE_TEXT:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _CGA6_M1)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_X16R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV03_GDI_RECTANGLE_TEXT:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _CGA6_M1)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_X24Y8)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV04_CONTEXT_COLOR_KEY:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_A16R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV04_RENDER_SOLID_LIN:
        case NV04_RENDER_SOLID_TRIANGLE:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_X16R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV04_INDEXED_IMAGE_FROM_CPU:
        case NV04_IMAGE_FROM_CPU:
        case NV04_STRETCHED_IMAGE_FROM_CPU:
        case NV10_IMAGE_FROM_CPU:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV05_SCALED_IMAGE_FROM_MEMORY:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_A1R5G5B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV05_INDEXED_IMAGE_FROM_CPU:
        case NV05_IMAGE_FROM_CPU:
        case NV05_STRETCHED_IMAGE_FROM_CPU:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV20_KELVIN_PRIMITIVE:
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);
            break;
        case NV10_TEXTURE_FROM_CPU:
            // TEXTURE_FROM_CPU is similar to the IMAGE_FROM_CPU classes, but with
            // only a subset of the methods, so we'll need to make sure things are
            // initialized in a way that makes sense for this class.
            ctx1 |= DRF_DEF(_PGRAPH, _CTX_SWITCH1, _PATCH_CONFIG, _SRCCOPY)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH1, _DITHER_MODE, _TRUNCATE);
            ctx2 = DRF_DEF(_PGRAPH, _CTX_SWITCH2, _MONO_FORMAT, _INVALID)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _COLOR_FORMAT, _LE_R5G6B5)
                | DRF_DEF(_PGRAPH, _CTX_SWITCH2, _NOTIFY_INSTANCE, _INVALID);

            break;
        default:
            // assume defaults are cool
            break;
    */            
    }


    //
    // Write out the instance stuff
    //
    
    INST_WR32(RegisterBase, Instance, SF_OFFSET(NV_PRAMIN_CONTEXT_0), ctx1);
    INST_WR32(RegisterBase, Instance, SF_OFFSET(NV_PRAMIN_CONTEXT_1), ctx2);
    INST_WR32(RegisterBase, Instance, SF_OFFSET(NV_PRAMIN_CONTEXT_2), ctx3);

    //
    // Clear this dword clear to keep the graphics method trapped disabled.
    //
    
    INST_WR32(RegisterBase, Instance, SF_OFFSET(NV_PRAMIN_CONTEXT_3), 0);
}


typedef struct _def_grctxpipe_nv2a          GRCTXPIPE_NV2A, *PGRCTXPIPE_NV2A;
typedef struct _def_grctxcmn_nv20           GRCTXCMN_NV20, *PGRCTXCMN_NV20;
typedef struct _def_grctx3d_nv20            GRCTX3D_NV20, *PGRCTX3D_NV20;


//
// The format of the graphics channel context is derived
// from /hw/nv20/docs/context_switching.doc.
//


//
// Pipe state.
//
#define XL_VAB_BASE         NV_IGRAPH_XF2PIPE(NV_IGRAPH_XF_CMD_VAB,0)
#define XL_XFCTX_BASE       NV_IGRAPH_XF2PIPE(NV_IGRAPH_XF_CMD_XFCTX,0)
#define XL_LTCTX_BASE       NV_IGRAPH_XF2PIPE(NV_IGRAPH_XF_CMD_LTCTX,0)
#define XL_LTC0_BASE        NV_IGRAPH_XF2PIPE(NV_IGRAPH_XF_CMD_LTC0,0)
#define XL_LTC1_BASE        NV_IGRAPH_XF2PIPE(NV_IGRAPH_XF_CMD_LTC1,0)
#define XL_LTC2_BASE        NV_IGRAPH_XF2PIPE(NV_IGRAPH_XF_CMD_LTC2,0)
#define XL_LTC3_BASE        NV_IGRAPH_XF2PIPE(NV_IGRAPH_XF_CMD_LTC3,0)

#define IDX_FMT_COUNT       (16+16+3)       // Vertex Array Regs+State
#define CAS0_COUNT          (6*4+5)         // Primitive Assembly Vtx0
#define CAS1_COUNT          (6*4+5)         // Primitive Assembly Vtx1
#define CAS2_COUNT          (6*4+5)         // Primitive Assembly Vtx2
#define ASSM_STATE_COUNT    (2)             // Primitive Assembly State
#define IDX_CACHE0_COUNT    (32*4)          // FD Coefficients (EvenLeft)
#define IDX_CACHE1_COUNT    (32*4)          // FD Coefficients (OddLeft)
#define IDX_CACHE2_COUNT    (32*4)          // FD Coefficients (EvenRight)
#define IDX_CACHE3_COUNT    (32*4)          // FD Coefficients (OddRight)
#define VTX_FILE0_COUNT     (48*4)          // Vertex File0
#define VTX_FILE1_COUNT     (120*4)         // Vertex File1 (NV2A)
#define XL_P_COUNT          (136*4)         // Cheops Program
#define XL_XFCTX_COUNT      (192*4)         // Cheops Context
#define XL_LTC0_COUNT       (4)             // Zoser Context 0
#define XL_LTC1_COUNT       (20)            // Zoser Context 1
#define XL_LTC2_COUNT       (15)            // Zoser Context 2
#define XL_LTC3_COUNT       (14)            // Zoser Context 3
#define XL_LTC_COUNT        (XL_LTC0_COUNT+XL_LTC1_COUNT+XL_LTC2_COUNT+XL_LTC3_COUNT)
#define XL_VAB_COUNT        (17*4)          // VAB
#define CRSTR_STIPP_COUNT   (32)            // RSTR stipple
#define FD_CTRL_COUNT       (15)            // FD
#define ROP_REGS_COUNT      (1)             // ROP Regs


#define XL_LTCTXA_COUNT           (26*4)    // (NV2A)
#define XL_LTCTXB_COUNT           (52*4)    // (NV2A)

struct _def_grctxpipe_nv2a
{
    ULONG IdxFmt[IDX_FMT_COUNT];
    ULONG Cas0[CAS0_COUNT];
    ULONG Cas1[CAS1_COUNT];
    ULONG Cas2[CAS2_COUNT];
    ULONG AssmState[ASSM_STATE_COUNT];
    ULONG IdxCache0[IDX_CACHE0_COUNT];
    ULONG IdxCache1[IDX_CACHE1_COUNT];
    ULONG IdxCache2[IDX_CACHE2_COUNT];
    ULONG IdxCache3[IDX_CACHE3_COUNT];
    ULONG VtxFile0[VTX_FILE0_COUNT];
    ULONG VtxFile1[VTX_FILE1_COUNT];
    ULONG XlP[XL_P_COUNT];
    ULONG XlXfCtx[XL_XFCTX_COUNT];
    ULONG XlLtCtxA[XL_LTCTXA_COUNT];
    ULONG XlLtCtxB[XL_LTCTXB_COUNT];
    ULONG XlLtc[XL_LTC_COUNT];
    ULONG XlVab[XL_VAB_COUNT];
    ULONG CrstrStipp[CRSTR_STIPP_COUNT];
    ULONG FdCtrl[FD_CTRL_COUNT];
    ULONG RopRegs[ROP_REGS_COUNT];
    ULONG Pad[4];
};


//
// Because graphics channel context switching is done in hw
// on nv20, we aren't in control of the format of this data
// structure (be careful!).
//

//
// This structure represents the channel context state
// that is either 2d-only or shared between 2d/3d.
//
struct _def_grctxcmn_nv20
{
    ULONG ContextUser;
    ULONG ContextSwitch1;
    ULONG ContextSwitch2;
    ULONG ContextSwitch3;
    ULONG ContextSwitch4;
    ULONG ContextSwitch5;
    ULONG ContextCache1[8];
    ULONG ContextCache2[8];
    ULONG ContextCache3[8];
    ULONG ContextCache4[8];
    ULONG ContextCache5[8];

    ULONG MonoColor0;
    ULONG BufferSwizzle2;
    ULONG BufferSwizzle5;
    ULONG BufferOffset[6];
    ULONG BufferBase[6];
    ULONG BufferPitch[5];
    ULONG BufferLimit[6];
    ULONG Chroma;
    ULONG Surface;
    ULONG State;
    ULONG Notify;
    ULONG BufferPixel;
    ULONG DmaPitch;
    ULONG DvdColorFmt;
    ULONG ScaledFormat;
    ULONG PatternColor0;
    ULONG PatternColor1;
    ULONG Pattern[2];
    ULONG PatternShape;
    ULONG PattColorRam[64];
    ULONG Rop3;
    ULONG BetaAnd;
    ULONG BetaPreMult;
    ULONG StoredFmt;
    ULONG AbsXRam[10];
    ULONG AbsYRam[10];
    ULONG AbsUClipXMin;
    ULONG AbsUClipXMax;
    ULONG AbsUClipYMin;
    ULONG AbsUClipYMax;
    ULONG AbsUClipAXMin;
    ULONG AbsUClipAXMax;
    ULONG AbsUClipAYMin;
    ULONG AbsUClipAYMax;
    ULONG SourceColor;
    ULONG Misc24_0;
    ULONG XYLogicMisc0;
    ULONG XYLogicMisc1;
    ULONG XYLogicMisc2;
    ULONG XYLogicMisc3;
    ULONG ClipX0;
    ULONG ClipX1;
    ULONG ClipY0;
    ULONG ClipY1;
    ULONG AbsIClipYMax;
    ULONG AbsIClipXMax;
    ULONG Passthru0;
    ULONG Passthru1;
    ULONG Passthru2;
    ULONG DimxTexture;
    ULONG WdimxTexture;
    ULONG DmaStart0;
    ULONG DmaStart1;
    ULONG DmaLength;
    ULONG DmaMisc;
    ULONG Misc24_1;
    ULONG Misc24_2;
    ULONG XMisc;
    ULONG YMisc;
    ULONG Valid1;
};

//
// This structure represents 3d channel context state.
//
struct _def_grctx3d_nv20
{
    ULONG AntiAliasing;
    ULONG Blend;
    ULONG BlendColor;
    ULONG BorderColor0;
    ULONG BorderColor1;
    ULONG BorderColor2;
    ULONG BorderColor3;
    ULONG BumpMat00_1;
    ULONG BumpMat00_2;
    ULONG BumpMat00_3;
    ULONG BumpMat01_1;
    ULONG BumpMat01_2;
    ULONG BumpMat01_3;
    ULONG BumpMat10_1;
    ULONG BumpMat10_2;
    ULONG BumpMat10_3;
    ULONG BumpMat11_1;
    ULONG BumpMat11_2;
    ULONG BumpMat11_3;
    ULONG BumpOffset1;
    ULONG BumpOffset2;
    ULONG BumpOffset3;
    ULONG BumpScale1;
    ULONG BumpScale2;
    ULONG BumpScale3;
    ULONG ClearRectX;
    ULONG ClearRectY;
    ULONG ColorClearValue;
    ULONG ColorKeyColor0;
    ULONG ColorKeyColor1;
    ULONG ColorKeyColor2;
    ULONG ColorKeyColor3;
    ULONG CombineFactor0_0;
    ULONG CombineFactor0_1;
    ULONG CombineFactor0_2;
    ULONG CombineFactor0_3;
    ULONG CombineFactor0_4;
    ULONG CombineFactor0_5;
    ULONG CombineFactor0_6;
    ULONG CombineFactor0_7;
    ULONG CombineFactor1_0;
    ULONG CombineFactor1_1;
    ULONG CombineFactor1_2;
    ULONG CombineFactor1_3;
    ULONG CombineFactor1_4;
    ULONG CombineFactor1_5;
    ULONG CombineFactor1_6;
    ULONG CombineFactor1_7;
    ULONG CombineAlphaI0;
    ULONG CombineAlphaI1;
    ULONG CombineAlphaI2;
    ULONG CombineAlphaI3;
    ULONG CombineAlphaI4;
    ULONG CombineAlphaI5;
    ULONG CombineAlphaI6;
    ULONG CombineAlphaI7;
    ULONG CombineAlphaO0;
    ULONG CombineAlphaO1;
    ULONG CombineAlphaO2;
    ULONG CombineAlphaO3;
    ULONG CombineAlphaO4;
    ULONG CombineAlphaO5;
    ULONG CombineAlphaO6;
    ULONG CombineAlphaO7;
    ULONG CombineColorI0;
    ULONG CombineColorI1;
    ULONG CombineColorI2;
    ULONG CombineColorI3;
    ULONG CombineColorI4;
    ULONG CombineColorI5;
    ULONG CombineColorI6;
    ULONG CombineColorI7;
    ULONG CombineColorO0;
    ULONG CombineColorO1;
    ULONG CombineColorO2;
    ULONG CombineColorO3;
    ULONG CombineColorO4;
    ULONG CombineColorO5;
    ULONG CombineColorO6;
    ULONG CombineColorO7;
    ULONG CombineCtl;
    ULONG CombineSpecFog0;
    ULONG CombineSpecFog1;
    ULONG Control0;
    ULONG Control1;
    ULONG Control2;
    ULONG Control3;
    ULONG FogColor;
    ULONG FogParam0;
    ULONG FogParam1;
    ULONG PointSize;
    ULONG SetupRaster;
    ULONG ShaderClipMode;
    ULONG ShaderCtl;
    ULONG ShaderProg;
    ULONG SemaphoreOffset;
    ULONG ShadowCtl;
    ULONG ShadowZSlopeThreshold;
    ULONG SpecFogFactor0;
    ULONG SpecFogFactor1;
    ULONG SurfaceClipX;
    ULONG SurfaceClipY;
    ULONG TexAddress0;
    ULONG TexAddress1;
    ULONG TexAddress2;
    ULONG TexAddress3;
    ULONG TexCtl0_0;
    ULONG TexCtl0_1;
    ULONG TexCtl0_2;
    ULONG TexCtl0_3;
    ULONG TexCtl1_0;
    ULONG TexCtl1_1;
    ULONG TexCtl1_2;
    ULONG TexCtl1_3;
    ULONG TexCtl2_0;
    ULONG TexCtl2_1;
    ULONG TexFilter0;
    ULONG TexFilter1;
    ULONG TexFilter2;
    ULONG TexFilter3;
    ULONG TexFormat0;
    ULONG TexFormat1;
    ULONG TexFormat2;
    ULONG TexFormat3;
    ULONG TexImageRect0;
    ULONG TexImageRect1;
    ULONG TexImageRect2;
    ULONG TexImageRect3;
    ULONG TexOffset0;
    ULONG TexOffset1;
    ULONG TexOffset2;
    ULONG TexOffset3;
    ULONG TexPallete0;
    ULONG TexPallete1;
    ULONG TexPallete2;
    ULONG TexPallete3;
    ULONG WindowClipX[8];
    ULONG WindowClipY[8];
    ULONG ZCompressOcclude;
    ULONG ZStencilClearValue;
    ULONG ZClipMax;
    ULONG ZClipMin;
    ULONG ContextDmaA;
    ULONG ContextDmaB;
    ULONG ContextDmaVtxA;
    ULONG ContextDmaVtxB;
    ULONG ZOffsetBias;
    ULONG ZOffsetFactor;
    ULONG Eyevec0;
    ULONG Eyevec1;
    ULONG Eyevec2;
    ULONG Shadow;
    ULONG FdData;
    ULONG FdSwatch;
    ULONG FdExtras;
    ULONG Emission0;
    ULONG Emission1;
    ULONG Emission2;
    ULONG SceneAmb0;
    ULONG SceneAmb1;
    ULONG SceneAmb2;
    ULONG ContextDmaGetState;
    ULONG BeginPatch0;
    ULONG BeginPatch1;
    ULONG BeginPatch2;
    ULONG BeginPatch3;
    ULONG BeginCurve;
    ULONG BeginTrans0;
    ULONG BeginTrans1;
    ULONG Csv0_D;
    ULONG Csv0_C;
    ULONG Csv1_A;
    ULONG Csv1_B;
    ULONG CheopsOffset;
};

struct _def_graphics_channel_nv20
{
    GRCTXCMN_NV20   grCtxCmn;
    GRCTX3D_NV20    grCtx3d;
    GRCTXPIPE_NV2A  grCtxPipe;
};


VOID
CMiniport::HalGrInit3d()
{


    BYTE* RegisterBase = (BYTE*)m_RegisterBase;

    ULONG grCtxBuffer;
    ULONG ctxUser;
    ULONG zoserC0Offset, zoserC1Offset;
    ULONG i;

    grCtxBuffer = m_HalInfo.GrCtxTable[m_HalInfo.FifoChID];

    //
    // Initialize 3d channel state.
    // Start by setting 3D present bit in CTX_USER
    // (offset 0 in ctx buffer).
    //
    ctxUser = INST_RD32(RegisterBase, grCtxBuffer, 0);
    ctxUser |= DRF_DEF(_PGRAPH, _CTX_USER, _CHANNEL_3D, _TRUE);
    INST_WR32(RegisterBase, grCtxBuffer, 0, ctxUser);

    //
    // Offset starts after state common to 2d and 3d.
    //
    ULONG offset = sizeof (GRCTXCMN_NV20)/4;

    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0xffff0000);   //Antialiasing
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //Blend
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BlendColor
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BorderColor0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BorderColor1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BorderColor2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BorderColor3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BumpMat00_1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BumpMat00_2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BumpMat00_3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BumpMat01_1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BumpMat01_2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BumpMat01_3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BumpMat10_1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BumpMat10_2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BumpMat10_3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BumpMat11_1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BumpMat11_2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BumpMat11_3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BumpOffset1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BumpOffset2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BumpOffset3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BumpScale1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BumpScale2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BumpScale3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x0fff0000);   //ClearRectX
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x0fff0000);   //ClearRectY
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ColorClearValue
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ColorKeyColor0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ColorKeyColor1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ColorKeyColor2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ColorKeyColor3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineFactor0_0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineFactor0_1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineFactor0_2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineFactor0_3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineFactor0_4
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineFactor0_5
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineFactor0_6
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineFactor0_7
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineFactor1_0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineFactor1_1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineFactor1_2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineFactor1_3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineFactor1_4
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineFactor1_5
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineFactor1_6
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineFactor1_7
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineAlphaI0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineAlphaI1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineAlphaI2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineAlphaI3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineAlphaI4
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineAlphaI5
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineAlphaI6
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineAlphaI7
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineAlphaO0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineAlphaO1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineAlphaO2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineAlphaO3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineAlphaO4
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineAlphaO5
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineAlphaO6
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineAlphaO7
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineColorI0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineColorI1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineColorI2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineColorI3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineColorI4
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineColorI5
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineColorI6
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineColorI7
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineColorO0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineColorO1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineColorO2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineColorO3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineColorO4
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineColorO5
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineColorO6
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineColorO7
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x101);        //CombineCtl
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineSpecFog0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CombineSpecFog1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //Control0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //Control1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x111);        //Control2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //Control3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //FogColor
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //FogParam0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //FogParam1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //PointSize
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x44400000);   //SetupRaster
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ShaderClipMode
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ShaderCtl
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ShaderProg
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //SemaphoreOffset
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ShadowCtl
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ShadowZslopeThreshold
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //SpecFogFactor0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //SpecFogFactor1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //SurfaceClipX
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //SurfaceClipY
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x30303);      //TexAddress0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x30303);      //TexAddress1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x30303);      //TexAddress2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x30303);      //TexAddress3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //TexControl0_0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //TexControl0_1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //TexControl0_2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //TexControl0_3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x80000);      //TexControl1_0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x80000);      //TexControl1_1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x80000);      //TexControl1_2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x80000);      //TexControl1_3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //TexControl2_0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //TexControl2_1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x01012000);   //TexFilter0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x01012000);   //TexFilter1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x01012000);   //TexFilter2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x01012000);   //TexFilter3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x105b8);      //TexFormat0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x105b8);      //TexFormat1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x105b8);      //TexFormat2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x105b8);      //TexFormat3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00080008);   //TexImageRect0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00080008);   //TexImageRect1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00080008);   //TexImageRect2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00080008);   //TexImageRect3

    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //TexOffset0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //TexOffset1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //TexOffset2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //TexOffset3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //TexPalette0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //TexPalette1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //TexPalette2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //TexPalette3

    for (i = 0; i < 8; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x07ff0000);    //WindowClipX

    for (i = 0; i < 8; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x07ff0000);    //WindowClipY

    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ZCompressOcclude
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ZStencilClearValue
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x4b7fffff);   //ZClipMax
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ZClipMin
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ContextDmaA
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ContextDmaB
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ContextDmaVtxA
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ContextDmaVtxB
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ZOffsetBias
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ZOffsetFactor
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //Eyevec0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //Eyevec1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //Eyevec2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //Shadow
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //FdData
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //FdSwatch
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //FdExtras
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //Emission0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //Emission1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //Emission2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //SceneAmb0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //SceneAmb1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //SceneAmb2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //ContextDmaGetState
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x1);          //BeginPatch0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BeginPatch1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x4000);       //BeginPatch2
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BeginPatch3
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BeginCurve
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x1);          //BeginTrans0
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //BeginTrans1
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00040000);   //Csv0_D
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00010000);   //Csv0_C
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //Csv1_B
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //Csv1_A
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);            //CheopsOffset

    ASSERT(offset == (sizeof(GRCTX3D_NV20) + sizeof(GRCTXCMN_NV20)) / 4);

    for (i = 0; i < IDX_FMT_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //IDX_FMT
    for (i = 0; i < CAS0_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //CAS0
    for (i = 0; i < CAS1_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //CAS1
    for (i = 0; i < CAS2_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //CAS2
    for (i = 0; i < ASSM_STATE_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //ASSM_STATE
    for (i = 0; i < IDX_CACHE0_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //IDX_CACHE0
    for (i = 0; i < IDX_CACHE1_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //IDX_CACHE1
    for (i = 0; i < IDX_CACHE2_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //IDX_CACHE2
    for (i = 0; i < IDX_CACHE3_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //IDX_CACHE3
    for (i = 0; i < VTX_FILE0_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //VTX_FILE0
    for (i = 0; i < VTX_FILE1_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //VTX_FILE1
    for (i = 0; i < XL_P_COUNT; i+=4)
    {
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x10700ff9); //XL_P
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x0436086c);
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x000c001b);
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    }

    ULONG cheopsCtxOffset = offset;
    for (i = 0; i < XL_XFCTX_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //XL_XFCTX

    ULONG zoserCtxAoffset = offset; 
    for (i = 0; i < XL_LTCTXA_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //XL_LTCTXA
    
    for (i = 0; i < XL_LTCTXB_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //XL_LTCTXB

    zoserC0Offset = offset;
    for (i = 0; i < XL_LTC0_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //XL_LTC0

    zoserC1Offset = offset;
    for (i = 0; i < XL_LTC1_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //XL_LTC1
    for (i = 0; i < XL_LTC2_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //XL_LTC2
    for (i = 0; i < XL_LTC3_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //XL_LTC3
    for (i = 0; i < XL_VAB_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //XL_VAB
    for (i = 0; i < CRSTR_STIPP_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //CRSTR_STIPP
    for (i = 0; i < FD_CTRL_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //FD_CTRL
    for (i = 0; i < ROP_REGS_COUNT; i++)
        INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0);         //ROP_REGS

    ASSERT(offset + 4 == (sizeof(GRCTX3D_NV20) 
                        + sizeof(GRCTXCMN_NV20)
                        + sizeof(GRCTXPIPE_NV2A)) / 4);

    // CHEOP_CTX, eye pos
    offset = cheopsCtxOffset + (NV_IGRAPH_XF_XFCTX_EYEP<<2);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x3f800000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);

    // CHEOP_CTX_CONST0
    offset = cheopsCtxOffset + (NV_IGRAPH_XF_XFCTX_CONS0<<2);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x40000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x3f800000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x3f000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);

    // CHEOP_CTX_CONST1
    offset = cheopsCtxOffset + (NV_IGRAPH_XF_XFCTX_CONS1<<2);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x40000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x3f800000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0xbf800000);

    // CHEOP_CTX_CONST2
    offset = cheopsCtxOffset + (NV_IGRAPH_XF_XFCTX_CONS2<<2);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0xbf800000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);

    // CHEOP_CTX, fog plane
    offset = cheopsCtxOffset + (NV_IGRAPH_XF_XFCTX_FOG<<2);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x3f800000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);

    // CHEOP_CTX_CONST3
    offset = cheopsCtxOffset + (NV_IGRAPH_XF_XFCTX_CONS3<<2);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);

    // ZOSER_CTX_ZERO
    offset = zoserCtxAoffset + (NV_IGRAPH_XF_LTCTXA_ZERO<<2);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);

    // ZOSER_EYE_DIR (22bit words) -- see Simon Moy about this
    offset = zoserCtxAoffset + (NV_IGRAPH_XF_LTCTXA_EYED<<2);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x3f800000>>10);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);

    // ZOSER_FRONT_AMBIENT1
    offset = zoserCtxAoffset + (NV_IGRAPH_XF_LTCTXA_FR_AMB<<2);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);

    // ZOSER_FRONT_AMBIENT2
    offset = zoserCtxAoffset + (NV_IGRAPH_XF_LTCTXA_CM_COL<<2);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);

    // ZOSER_EYE_DIR (22bit words) -- see Simon Moy about this
    offset = zoserCtxAoffset + (NV_IGRAPH_XF_LTCTXA_FOGLIN<<2);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x3f800000>>20);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);
    INST_WR32(RegisterBase, grCtxBuffer, offset++<<2, 0x00000000);

    // ZOSER_C0 (22bit word) -- see Simon Moy about this
    offset = zoserC0Offset + (NV_IGRAPH_XF_LTC0_MONE);
    INST_WR32(RegisterBase, grCtxBuffer, offset<<2, 0xbf800000>>10);

    // LIGHT0 range (22bit word) -- see Simon Moy about this
    offset = zoserC1Offset + (NV_IGRAPH_XF_LTC1_r0);
    INST_WR32(RegisterBase, grCtxBuffer, offset<<2, 0x7149f2ca>>10);

    // LIGHT1 range (22bit word) -- see Simon Moy about this
    offset = zoserC1Offset + (NV_IGRAPH_XF_LTC1_r1);
    INST_WR32(RegisterBase, grCtxBuffer, offset<<2, 0x7149f2ca>>10);

    // LIGHT2 range (22bit word) -- see Simon Moy about this
    offset = zoserC1Offset + (NV_IGRAPH_XF_LTC1_r2);
    INST_WR32(RegisterBase, grCtxBuffer, offset<<2, 0x7149f2ca>>10);

    // LIGHT3 range (22bit word) -- see Simon Moy about this
    offset = zoserC1Offset + (NV_IGRAPH_XF_LTC1_r3);
    INST_WR32(RegisterBase, grCtxBuffer, offset<<2, 0x7149f2ca>>10);

    // LIGHT4 range (22bit word) -- see Simon Moy about this
    offset = zoserC1Offset + (NV_IGRAPH_XF_LTC1_r4);
    INST_WR32(RegisterBase, grCtxBuffer, offset<<2, 0x7149f2ca>>10);

    // LIGHT5 range (22bit word) -- see Simon Moy about this
    offset = zoserC1Offset + (NV_IGRAPH_XF_LTC1_r5);
    INST_WR32(RegisterBase, grCtxBuffer, offset<<2, 0x7149f2ca>>10);

    // LIGHT6 range (22bit word) -- see Simon Moy about this
    offset = zoserC1Offset + (NV_IGRAPH_XF_LTC1_r6);
    INST_WR32(RegisterBase, grCtxBuffer, offset<<2, 0x7149f2ca>>10);

    // LIGHT7 range (22bit word) -- see Simon Moy about this
    offset = zoserC1Offset + (NV_IGRAPH_XF_LTC1_r7);
    INST_WR32(RegisterBase, grCtxBuffer, offset<<2, 0x7149f2ca>>10);


    ASSERT(m_HalInfo.GrChID != (ULONG)m_HalInfo.FifoChID);

}

} // end of namespace

