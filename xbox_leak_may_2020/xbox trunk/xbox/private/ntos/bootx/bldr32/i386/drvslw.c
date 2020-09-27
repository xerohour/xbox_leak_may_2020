/*++

Copyright (c) 1989-2001  Microsoft Corporation

Module Name:

    drvslw.c

Abstract:

    This module implements code to perform Drive/Slew Pad compensation, so we
    can bring the memory clock up to fullspeed early during boot.

--*/

#include "bldr32.h"
#include <bldr.h>
#include "romdata.h"
#include <nv_ref_2a.h>

//
// To enable cache checking code using PMC, define CHECKCACHE
//
#undef CHECKCACHE

//
// Location of the ROM base (mapped in BldrStartup) and the datatbl offset.
//
#define BLDR_ROM_BASE               0xFFC00000
#define BLDR_ROM_DATA_OFFSET        0x0000007C

//
// Macros used by calibration code
//
#define TOFIXEDPOINT(d)             ((d) << 20)
#define SLW_VALUE(r,f)              SlwValue(r,f)
#define FROMFIXEDPOINT(d)           FromFixedPoint(d);

#define NV_PBUS_FBIO_CALEN_VAL      0x000f0f0f
#define NV_PBUS_FBIO_CALSEL_VCCQ    0x00040004
#define NV_PBUS_FBIO_CALSEL_VCC     0x00040000

#define DRF_MASK(drf)               (0xFFFFFFFF>>(31-((1?drf) % 32)+((0?drf) % 32)))

//
// PMC event to use for cache checking code
//
#define PMC_EVENT    0x65   // Number of bus burst read transactions

//
// LDT flow control enable bits
//
#define LDT_FLOW_CONTROL_BITS       0x88000000


//
// LDT registers
//
#define CR_CPU_LDT_1                0x80000064
#define CR_CPU_LDT_3_USER_CNFG      0x8000006C
#define MCP_LEG_CFG_21              0x80000854
 

#define CR_CPU_MEMTOP               0x80000084
#define CR_CPU_MEMTOP_LIMIT_64MB    0x03FFFFFF
#define CR_CPU_MEMTOP_LIMIT_128MB   0x07FFFFFF

#define CR_CPU_GRAP                 0x80000080

//
// Memory test results
//
#define MCP_USBA_CFG_15             0x8000103C
#define MCP_USBB_CFG_15             0x8000183C



typedef struct _REGVALUEPAIR {
    ULONG Register;
    ULONG Value;
} REGVALUEPAIR, *PREGVALUEPAIR;



FORCEINLINE
ULONG
REG_RD32(
    VOID* Ptr,
    ULONG Addr
    )
{
    return *((volatile ULONG*)((UCHAR*)(Ptr) + (Addr)));
}


FORCEINLINE
VOID
REG_WR32(
    VOID* Ptr,
    ULONG Addr,
    ULONG Val)
{
    *((volatile ULONG*)((UCHAR*)(Ptr) + (Addr))) = (ULONG)(Val);
}


#ifdef CHECKCACHE
FORCEINLINE
VOID
SetPMCEvent(
    ULONG Event
    )
{
    Event |= ((1 << 22) | (1 << 17) | (1 << 18) | (1 << 20));

    __asm {
        mov     ecx, 0x186
        mov     eax, Event
        xor     edx, edx
        wrmsr  

        ;
        ; mov eax, cr4
        ;
        _emit   0Fh
        _emit   20h
        _emit   0E0h  

        or eax, 100h
        
        ;
        ; mov cr4, eax
        ;
        _emit   0Fh
        _emit   22h
        _emit   0E0h  
    }
}


FORCEINLINE
ULONG
ReadPMCLow(
    VOID
    )
{
    ULONG LowPart;

    __asm {
        xor     edx, edx
        xor     ecx, ecx
        _emit   0fh
        _emit   33h
        mov     LowPart, eax
    }
    
    return LowPart;
};
#endif


VOID 
BldrDelay(
    ULONG nsec
    )
/*++

Routine Description:

    This is a busy wait loop for the drive/slew calibration only.  Its not very 
    accurate, since the 733MHz cycle time (1.364ns) is rounded to 1ns and also 
    doesn't account for the instructions that make-up the loop.  The idle times 
    being passed in are currently 10us, so delaying a little longer in this 
    routine shouldn't be noticeable.


Arguments:


Return Value:

    None.

--*/
{
    ULONG tick_curr_lo, tick_curr_hi;
    ULONG tick_end_lo, tick_end_hi;

    //
    // Read start counters
    //
    __asm {
        rdtsc
        mov tick_curr_lo, eax
        mov tick_curr_hi, edx
    }

    tick_end_hi = tick_curr_hi;
    tick_end_lo = tick_curr_lo + nsec;  

    // 
    // Handle rollover
    //
    if (tick_end_lo < tick_curr_lo) {
        tick_end_hi++;  
    }

    //
    // Do nothing loop for specified time
    //
    do {
        __asm {
            rdtsc
            mov tick_curr_lo, eax
            mov tick_curr_hi, edx
        }
    } while ((tick_curr_hi <= tick_end_hi) &&
             (tick_curr_lo < tick_end_lo)); 
}


static _declspec(noinline)
ULONG 
SlwValue(
    ULONG r, 
    ULONG f
    )
{
   return ((r << 28) | (f << 24) | (r << 20) | (f << 16) | (r << 12) | (f << 8)  | (r <<  4) |  f);
}


static _declspec(noinline)
ULONG 
FromFixedPoint(
    ULONG d
    )
{
    return ((((d) + (0x1 << 19)) >> 20) & 0xf);
}



FORCEINLINE
ULONG
ReadPCIConfig(
    ULONG Reg
    )
{
    _outpd(PCI_TYPE1_ADDR_PORT, Reg);
    return _inpd(PCI_TYPE1_DATA_PORT);
}


VOID
BldrSetDrvSlwFromROMData(
    PVOID RegisterBase
    )
{
    ULONG *offsetp;
    PROM_DATATBL pROMData;
    PDRVSLWCALPARAMS pDrvSlwCalParams;
    PDRVSLWPADPARAMS pPadParams, pDrvSlwPadParam_lo, pDrvSlwPadParam_hi;
    ULONG adr_drv_fall, adr_drv_rise, adr_drv,
          clk_drv_fall, clk_drv_rise, clk_drv,
          dat_drv_fall, dat_drv_rise, dat_drv,
          dqs_drv_fall, dqs_drv_rise, dqs_drv;
    ULONG adr_slw_fall, adr_slw_rise, adr_slw,
          clk_slw_fall, clk_slw_rise, clk_slw,
          dat_slw_fall, dat_slw_rise, dat_slw,
          dqs_slw_fall, dqs_slw_rise, dqs_slw;
    ULONG dqs_ib_del, data_ib_del, clk_ic_del, fbio_dly;
    ULONG vccq_a, vccq_b, vccq_c, vccq_d, avg_vccq;
    ULONG vcc_a, vcc_b, vcc_c, vcc_d, avg_vcc;
    ULONG ratio_a, ratio_b;

    REGVALUEPAIR RegValuePair[9];
    ULONG Pass;
    ULONG i;
#ifdef CHECKCACHE
    ULONG BeginHits;
    ULONG EndHits;
#endif
    ULONG StrapEMRS;

    //
    // Find start of ROM datatbl containing drive/slew data
    //
    offsetp = (ULONG *)(BLDR_ROM_BASE + BLDR_ROM_DATA_OFFSET);
    pROMData = (PROM_DATATBL)(BLDR_ROM_BASE + *offsetp);
    pDrvSlwCalParams = &pROMData->calParams;

    //
    // Check to see if its Samsung or a Micron memory bits 19:18
    //
    StrapEMRS = REG_RD32(RegisterBase, NV_PEXTDEV_BOOT_0) & 0x0000C0000;

    if (StrapEMRS == 0) {
        pPadParams = &(pROMData->MicronParams[0]);
        BldrPrint(("Bldr: Memory type: Micron"));
    }
    else {
        pPadParams = &(pROMData->SamsungParams[0]);
        BldrPrint(("Bldr: Memory type: Samsung"));
    }
    
#ifdef BLDRSPEW
    //
    // Dump out memory sizes etc
    //
    if (ReadPCIConfig(CR_CPU_MEMTOP) == CR_CPU_MEMTOP_LIMIT_64MB) {
        BldrPrint(("Bldr: Memory size: 64MB"));
    } else {
        BldrPrint(("Bldr: Memory size: 128MB")); 
    }
    
    BldrPrint(("Bldr: Memory test results: 4MB/2MB=%x, Pass/Fail=%x", 
        ReadPCIConfig(MCP_USBA_CFG_15) & 0xff, 
        ReadPCIConfig(MCP_USBB_CFG_15) & 0xff));
    
    BldrPrint(("Bldr: NV_PBUS_FBIO_DLY=%x", REG_RD32(RegisterBase, NV_PBUS_FBIO_DLY)));
    BldrPrint(("Bldr: PadParams=%x", pPadParams));

    BldrPrint(("Bldr: DriveSlewCalibration started"));
#endif    

    //
    // Read the calibration for outbound/inbound 
    //
    REG_WR32(RegisterBase, NV_PBUS_FBIO_CALEN, NV_PBUS_FBIO_CALEN_VAL);

    REG_WR32(RegisterBase, NV_PBUS_FBIO_CALSEL, NV_PBUS_FBIO_CALSEL_VCCQ);
    BldrDelay(10*1000);    // 10 usecs
    vccq_a = REG_RD32(RegisterBase, NV_PBUS_FBIO_CALCNT);

    REG_WR32(RegisterBase, NV_PBUS_FBIO_CALSEL, NV_PBUS_FBIO_CALSEL_VCCQ+1);
    BldrDelay(10*1000);    // 10 usecs
    vccq_b = REG_RD32(RegisterBase, NV_PBUS_FBIO_CALCNT);

    REG_WR32(RegisterBase, NV_PBUS_FBIO_CALSEL, NV_PBUS_FBIO_CALSEL_VCCQ+2);
    BldrDelay(10*1000);    // 10 usecs
    vccq_c = REG_RD32(RegisterBase, NV_PBUS_FBIO_CALCNT);

    REG_WR32(RegisterBase, NV_PBUS_FBIO_CALSEL, NV_PBUS_FBIO_CALSEL_VCCQ+3);
    BldrDelay(10*1000);    // 10 usecs
    vccq_d = REG_RD32(RegisterBase, NV_PBUS_FBIO_CALCNT);

    avg_vccq = (vccq_a + vccq_b + vccq_c + vccq_d) / 4;

    BldrPrint(("Bldr: DriveSlewCalibration VCCQ_A=0x%x", vccq_a));
    BldrPrint(("Bldr: DriveSlewCalibration VCCQ_B=0x%x", vccq_b));
    BldrPrint(("Bldr: DriveSlewCalibration VCCQ_C=0x%x", vccq_c));
    BldrPrint(("Bldr: DriveSlewCalibration VCCQ_D=0x%x", vccq_d));

    REG_WR32(RegisterBase, NV_PBUS_FBIO_CALSEL, NV_PBUS_FBIO_CALSEL_VCC);
    BldrDelay(10*1000);    // 10 usecs
    vcc_a = REG_RD32(RegisterBase, NV_PBUS_FBIO_CALCNT);

    REG_WR32(RegisterBase, NV_PBUS_FBIO_CALSEL, NV_PBUS_FBIO_CALSEL_VCC+1);
    BldrDelay(10*1000);    // 10 usecs
    vcc_b = REG_RD32(RegisterBase, NV_PBUS_FBIO_CALCNT);

    REG_WR32(RegisterBase, NV_PBUS_FBIO_CALSEL, NV_PBUS_FBIO_CALSEL_VCC+2);
    BldrDelay(10*1000);    // 10 usecs
    vcc_c = REG_RD32(RegisterBase, NV_PBUS_FBIO_CALCNT);

    REG_WR32(RegisterBase, NV_PBUS_FBIO_CALSEL, NV_PBUS_FBIO_CALSEL_VCC+3);
    BldrDelay(10*1000);    // 10 usecs
    vcc_d = REG_RD32(RegisterBase, NV_PBUS_FBIO_CALCNT);

    avg_vcc = (vcc_a + vcc_b + vcc_c + vcc_d) / 4;

    BldrPrint(("Bldr: DriveSlewCalibration VCC_A=0x%x", vcc_a));
    BldrPrint(("Bldr: DriveSlewCalibration VCC_B=0x%x", vcc_b));
    BldrPrint(("Bldr: DriveSlewCalibration VCC_C=0x%x", vcc_c));
    BldrPrint(("Bldr: DriveSlewCalibration VCC_D=0x%x", vcc_d));

    REG_WR32(RegisterBase, NV_PBUS_FBIO_CALEN, 0);

    //
    // Determine outbound process params and interp ratio_a/ratio_b
    //
    if (avg_vccq <= pDrvSlwCalParams->SlowCountAvg) {
        BldrPrint(("Bldr: Outbound interp slow ext -> slow avg"));

        pDrvSlwPadParam_lo = &pPadParams[DrvSlwExtSlow];
        pDrvSlwPadParam_hi = &pPadParams[DrvSlwAvgSlow];

        ratio_a = TOFIXEDPOINT(avg_vccq - pDrvSlwCalParams->SlowCountExt) /
                              (pDrvSlwCalParams->SlowCountAvg - pDrvSlwCalParams->SlowCountExt);
        if (ratio_a & 0x80000000)  
            ratio_a = 0;
    
    } else if (avg_vccq >= pDrvSlwCalParams->FastCountAvg) {
        BldrPrint(("Bldr: Outbound interp fast avg -> fast ext"));

        pDrvSlwPadParam_lo = &pPadParams[DrvSlwAvgFast];
        pDrvSlwPadParam_hi = &pPadParams[DrvSlwExtFast];

        ratio_a = TOFIXEDPOINT(avg_vccq - pDrvSlwCalParams->FastCountAvg) /
                              (pDrvSlwCalParams->FastCountExt - pDrvSlwCalParams->FastCountAvg);
        if (ratio_a > TOFIXEDPOINT(1))
            ratio_a = 1;
    
    } else if (avg_vccq < pDrvSlwCalParams->TypiCount) {
        BldrPrint(("Bldr: Outbound interp slow avg -> typical"));

        pDrvSlwPadParam_lo = &pPadParams[DrvSlwAvgSlow];
        pDrvSlwPadParam_hi = &pPadParams[DrvSlwTypical];

        ratio_a = TOFIXEDPOINT(avg_vccq - pDrvSlwCalParams->SlowCountAvg) /
                              (pDrvSlwCalParams->TypiCount - pDrvSlwCalParams->SlowCountAvg);
    } else {
        BldrPrint(("Bldr: Outbound interp typical -> fast avg"));

        pDrvSlwPadParam_lo = &pPadParams[DrvSlwTypical];
        pDrvSlwPadParam_hi = &pPadParams[DrvSlwAvgFast];

        ratio_a = TOFIXEDPOINT(avg_vccq - pDrvSlwCalParams->TypiCount) /
                              (pDrvSlwCalParams->FastCountAvg - pDrvSlwCalParams->TypiCount);
    }

    ratio_b = TOFIXEDPOINT(1) - ratio_a;

    adr_drv_fall = FROMFIXEDPOINT((pDrvSlwPadParam_lo->AdrDrvFall * ratio_b) +
                                  (pDrvSlwPadParam_hi->AdrDrvFall * ratio_a)); 
    adr_drv_rise = FROMFIXEDPOINT((pDrvSlwPadParam_lo->AdrDrvRise * ratio_b) +
                                  (pDrvSlwPadParam_hi->AdrDrvRise * ratio_a));
    adr_slw_fall = FROMFIXEDPOINT((pDrvSlwPadParam_lo->AdrSlwFall * ratio_b) +
                                  (pDrvSlwPadParam_hi->AdrSlwFall * ratio_a));
    adr_slw_rise = FROMFIXEDPOINT((pDrvSlwPadParam_lo->AdrSlwRise * ratio_b) +
                                  (pDrvSlwPadParam_hi->AdrSlwRise * ratio_a));

    clk_drv_fall = FROMFIXEDPOINT((pDrvSlwPadParam_lo->ClkDrvFall * ratio_b) +
                                  (pDrvSlwPadParam_hi->ClkDrvFall * ratio_a));
    clk_drv_rise = FROMFIXEDPOINT((pDrvSlwPadParam_lo->ClkDrvRise * ratio_b) +
                                  (pDrvSlwPadParam_hi->ClkDrvRise * ratio_a));
    clk_slw_fall = FROMFIXEDPOINT((pDrvSlwPadParam_lo->ClkSlwFall * ratio_b) +
                                  (pDrvSlwPadParam_hi->ClkSlwFall * ratio_a));
    clk_slw_rise = FROMFIXEDPOINT((pDrvSlwPadParam_lo->ClkSlwRise * ratio_b) +
                                  (pDrvSlwPadParam_hi->ClkSlwRise * ratio_a));

    dat_drv_fall = FROMFIXEDPOINT((pDrvSlwPadParam_lo->DatDrvFall * ratio_b) +
                                  (pDrvSlwPadParam_hi->DatDrvFall * ratio_a));
    dat_drv_rise = FROMFIXEDPOINT((pDrvSlwPadParam_lo->DatDrvRise * ratio_b) +
                                  (pDrvSlwPadParam_hi->DatDrvRise * ratio_a));
    dat_slw_fall = FROMFIXEDPOINT((pDrvSlwPadParam_lo->DatSlwFall * ratio_b) +
                                  (pDrvSlwPadParam_hi->DatSlwFall * ratio_a));
    dat_slw_rise = FROMFIXEDPOINT((pDrvSlwPadParam_lo->DatSlwRise * ratio_b) +
                                  (pDrvSlwPadParam_hi->DatSlwRise * ratio_a));

    dqs_drv_fall = FROMFIXEDPOINT((pDrvSlwPadParam_lo->DqsDrvFall * ratio_b) +
                                  (pDrvSlwPadParam_hi->DqsDrvFall * ratio_a));
    dqs_drv_rise = FROMFIXEDPOINT((pDrvSlwPadParam_lo->DqsDrvRise * ratio_b) +
                                  (pDrvSlwPadParam_hi->DqsDrvRise * ratio_a));
    dqs_slw_fall = FROMFIXEDPOINT((pDrvSlwPadParam_lo->DqsSlwFall * ratio_b) +
                                  (pDrvSlwPadParam_hi->DqsSlwFall * ratio_a));
    dqs_slw_rise = FROMFIXEDPOINT((pDrvSlwPadParam_lo->DqsSlwRise * ratio_b) +
                                  (pDrvSlwPadParam_hi->DqsSlwRise * ratio_a));

    //
    // Determine inbound process params and interp ratio_a/ratio_b
    //
    if (avg_vcc <= pDrvSlwCalParams->SlowCountBAvg) {
        
        BldrPrint(("Bldr: Inbound interp slow ext -> slow avg"));

        pDrvSlwPadParam_lo = &pPadParams[DrvSlwExtSlow];
        pDrvSlwPadParam_hi = &pPadParams[DrvSlwAvgSlow];

        ratio_a = TOFIXEDPOINT(avg_vcc - pDrvSlwCalParams->SlowCountBExt) /
                              (pDrvSlwCalParams->SlowCountBAvg - pDrvSlwCalParams->SlowCountBExt);
        if (ratio_a & 0x80000000) {      
            ratio_a = 0;
        }
    
    } else if (avg_vcc >= pDrvSlwCalParams->FastCountBAvg) {
        
        BldrPrint(("Bldr: Inbound interp fast avg -> fast ext"));

        pDrvSlwPadParam_lo = &pPadParams[DrvSlwAvgFast];
        pDrvSlwPadParam_hi = &pPadParams[DrvSlwExtFast];

        ratio_a = TOFIXEDPOINT(avg_vcc - pDrvSlwCalParams->FastCountBAvg) /
                              (pDrvSlwCalParams->FastCountBExt - pDrvSlwCalParams->FastCountBAvg);
        if (ratio_a > TOFIXEDPOINT(1)) {
            ratio_a = 1;
        }
    
    } else if (avg_vcc < pDrvSlwCalParams->TypiCountB) {
        
        BldrPrint(("Bldr: Inbound interp slow avg -> typical"));

        pDrvSlwPadParam_lo = &pPadParams[DrvSlwAvgSlow];
        pDrvSlwPadParam_hi = &pPadParams[DrvSlwTypical];

        ratio_a = TOFIXEDPOINT(avg_vcc - pDrvSlwCalParams->SlowCountBAvg) /
                              (pDrvSlwCalParams->TypiCountB - pDrvSlwCalParams->SlowCountBAvg);
    
    } else {

        BldrPrint(("Bldr: Inbound interp typical -> fast avg"));

        pDrvSlwPadParam_lo = &pPadParams[DrvSlwTypical];
        pDrvSlwPadParam_hi = &pPadParams[DrvSlwAvgFast];

        ratio_a = TOFIXEDPOINT(avg_vcc - pDrvSlwCalParams->TypiCountB) /
                              (pDrvSlwCalParams->FastCountBAvg - pDrvSlwCalParams->TypiCountB);
    }
    
    ratio_b = TOFIXEDPOINT(1) - ratio_a;

    dqs_ib_del = FROMFIXEDPOINT((pDrvSlwPadParam_lo->DqsInbDely * ratio_b) +
                                (pDrvSlwPadParam_hi->DqsInbDely * ratio_a));

    data_ib_del = FROMFIXEDPOINT((pDrvSlwPadParam_lo->DataInbDely * ratio_b) +
                                 (pDrvSlwPadParam_hi->DataInbDely * ratio_a));

    clk_ic_del = FROMFIXEDPOINT((pDrvSlwPadParam_lo->ClkIcDely * ratio_b) +
                                (pDrvSlwPadParam_hi->ClkIcDely * ratio_a));
    
    //
    // NOTE: We must write all 9 register values without accessing memory
    //       Therefore, all code and data must be present in the cache
    // 

    //
    // Clear the cache
    //
    __asm wbinvd;

    //
    // Save the final register values and registers in an array
    //

    adr_drv = SLW_VALUE(adr_drv_rise, adr_drv_fall);
    RegValuePair[0].Register = NV_PBUS_FBIO_ADRDRV; 
    RegValuePair[0].Value = adr_drv;  

    adr_slw = SLW_VALUE(adr_slw_rise, adr_slw_fall);
    RegValuePair[1].Register = NV_PBUS_FBIO_ADRSLW; 
    RegValuePair[1].Value = adr_slw;  

    clk_drv = SLW_VALUE(clk_drv_rise, clk_drv_fall);
    RegValuePair[2].Register = NV_PBUS_FBIO_CLKDRV; 
    RegValuePair[2].Value = clk_drv;

    clk_slw = SLW_VALUE(clk_slw_rise, clk_slw_fall);
    RegValuePair[3].Register = NV_PBUS_FBIO_CLKSLW; 
    RegValuePair[3].Value = clk_slw;  

    dat_drv = SLW_VALUE(dat_drv_rise, dat_drv_fall);
    RegValuePair[4].Register = NV_PBUS_FBIO_DATDRV; 
    RegValuePair[4].Value = dat_drv;  

    dat_slw = SLW_VALUE(dat_slw_rise, dat_slw_fall);
    RegValuePair[5].Register = NV_PBUS_FBIO_DATSLW; 
    RegValuePair[5].Value = dat_slw;  

    dqs_drv = SLW_VALUE(dqs_drv_rise, dqs_drv_fall);
    RegValuePair[6].Register = NV_PBUS_FBIO_DQSDRV; 
    RegValuePair[6].Value = dqs_drv;  

    dqs_slw = SLW_VALUE(dqs_slw_rise, dqs_slw_fall);
    RegValuePair[7].Register = NV_PBUS_FBIO_DQSSLW; 
    RegValuePair[7].Value = dqs_slw;  

    fbio_dly = (dqs_ib_del << 29) | (clk_ic_del << 27) | (data_ib_del << 24) |
               (dqs_ib_del << 21) | (clk_ic_del << 19) | (data_ib_del << 16) |
               (dqs_ib_del << 13) | (clk_ic_del << 11) | (data_ib_del << 8) |
               (dqs_ib_del << 5)  | (clk_ic_del << 3)  |  data_ib_del;
    
    RegValuePair[8].Register = NV_PBUS_FBIO_DLY; 
    RegValuePair[8].Value = fbio_dly;

    //
    // Read the array byte by byte to ensure it is in cache.  
    //
    for (i = 0; i < 9; i++) {
        if (RegValuePair[i].Register == 0 || RegValuePair[i].Value == 0) {
            RegValuePair[i].Register = 0;
        }
    }

#ifdef CHECKCACHE
    SetPMCEvent(PMC_EVENT);   
#endif
    
    //
    // First pass is a NOP and used to ensure the code is in cache
    // Second pass actually writes the register values out
    //
    for (Pass = 0; Pass < 2; Pass++) {

#ifdef CHECKCACHE
        BeginHits = ReadPMCLow();
#endif

        for (i = 0; i < 9; i++) {
            if (Pass == 1) {
                REG_WR32(RegisterBase, RegValuePair[i].Register, 
                    RegValuePair[i].Value);
            }

        }

#ifdef CHECKCACHE
        EndHits = ReadPMCLow();
#endif
    
    }
    
#ifdef CHECKCACHE
    BldrPrint(("Bldr: Mem reads during register writes=%d (must be zero) (%d,%d)", 
              (EndHits - BeginHits), EndHits, BeginHits));
#endif
    

#ifdef BLDRSPEW
    for (i = 0; i < 9; i++) {
        BldrPrint(("Bldr: DriveSlewCalibration register %x=%x", RegValuePair[i].Register, 
            RegValuePair[i].Value));
    }

    BldrPrint(("Bldr: DriveSlewCalibration setup finished"));
#endif

}




VOID
BldrSetupLDTBus(
    VOID
    )
{
    ULONG ldt_data;
    
    //
    // NOTE: Since we have to reset the LDT bus as part of this initialization, we
    // can't run this code out of the ROM (while resetting the bus, we won't be
    // able to execute the code to reenable it), so it has to be in memory.
    //
 
    BldrPrint(("Bldr: Enabling LDT DWORD flow control"));

    // 
    // Enable DWORD flow control in the MCP
    //
    _outpd(PCI_TYPE1_ADDR_PORT, MCP_LEG_CFG_21);
    ldt_data = _inpd(PCI_TYPE1_DATA_PORT);
    ldt_data |= LDT_FLOW_CONTROL_BITS;
    _outpd(PCI_TYPE1_DATA_PORT, ldt_data);
 
    //
    // Enable DWORD flow control in the NV2A
    //
    _outpd(PCI_TYPE1_ADDR_PORT, CR_CPU_LDT_1);
    ldt_data = _inpd(PCI_TYPE1_DATA_PORT);
    ldt_data |= LDT_FLOW_CONTROL_BITS;
    _outpd(PCI_TYPE1_DATA_PORT, ldt_data);
 
    //
    // Force a reset of the LDT link. This is done by toggling the
    // WARM_RESET bit (bit0), where a 0 initiates the reset and a 1 
    // completes it
    //
    _outpd(PCI_TYPE1_ADDR_PORT, CR_CPU_LDT_3_USER_CNFG);
    ldt_data = _inpd(PCI_TYPE1_DATA_PORT);
    _outpd(PCI_TYPE1_DATA_PORT, (ldt_data & 0xFFFFFFFE));
    _outpd(PCI_TYPE1_DATA_PORT, ldt_data);

    //
    // Allow the LDT->1GB transactions by ensuring that the GART is visible to LDT.
    // Due to the fact there is a shadow copy of this register, we must do this write
    // even though the power on value is already 0x100
    //
    _outpd(PCI_TYPE1_ADDR_PORT, CR_CPU_GRAP);
    _outpd(PCI_TYPE1_DATA_PORT, 0x100);
}
