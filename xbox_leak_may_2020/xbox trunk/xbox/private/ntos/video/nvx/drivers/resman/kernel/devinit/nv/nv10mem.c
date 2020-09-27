 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1999 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/**************************************************************************************************************
*
*	Module: nv10mem.c
*
*	Description:
*		Device-specific memory sizing code, for Nv10.
*
*	Revision History:
*		Original -- 6/99 Jeff Westerinen
*
**************************************************************************************************************/

#ifdef NV_PORTABLE

/* "portable" form for use outside resman and by external source customers */
/* look in devinit/portable/ for more details */
#include <nv_portable.h>
#include <nv_ref.h>
#include <nv10_ref.h>
#include <devinit.h>

#else	/* not NV_PORTABLE --> Nvidia internal */

#include <nvrm.h>
#include <nvhw.h>
#include <nv10_ref.h>
#include <os.h>
#include <devinit.h>

#endif

RM_STATUS Nv10MemConfigureClocks
(
    PHWINFO pDev,
    MEM_INIT_TABLE_NV10 memInitTable
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 clk;
    U032 memInitTableIndex;
    U032 M,N,P;
    U032 Head = 0; // straps are on head 0; likewise NVPLL, MPLL
/*
        ; Get straps, and turn AL into an index to the table
        ;                       
        mov     al,NV_CIO_SCRATCH4_INDEX        ; CR3C
        call    ReadCRTCReg                     ; Read current value
        shr     ax,12                           ; bits 0:3
*/
    // get the index in the memory init table from scratch register 4
    CRTC_RD(NV_CIO_CRE_SCRATCH4__INDEX, memInitTableIndex, Head);
    memInitTableIndex >>= 4;
    
/*
        ; Size of config structure
        mov     cl, SIZEOF MEM_INIT_VALS        ; Get the size of the config struct
        mul     cl
        
        ; Now point at the right structure with CS:SI
        mov     di, offset bmp_MemInitTbl
        add     di,ax                           ; Add offset into init table

        ; Program NVCLK first, since we've always done it that way
        ;

        mov     ax, cs:[di].MEM_INIT_VALS._NVClk_PLL    ; Get MCLK value
        push    bx      ; BX returns data from this call that we won't use
        call    CalcMNP                         ; Get MNP parameters
        pop     bx
        
        ;       AL = M parameter
        ;       AH = N parameter
        ;       DL = P parameter
        mov     esi,NV_PRAMDAC_NVPLL_COEFF
        call    ProgPLL                         ; Program the PLL's
*/
    // program NVCLK
#ifdef FORCE_NVCLOCKMHZ
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_WARNINGS, "NVRM: Nv10MemConfigureClocks: forcing NVCLK Mhz to ", FORCE_NVCLOCKMHZ);
    clk = FORCE_NVCLOCKMHZ * 100;	// Clock MHz * 100
#else
    clk = memInitTable[memInitTableIndex].NVClk_PLL;
#endif
// Use dacCalcMFP instead of DevinitCalcMNP,
// since the devinit version seems to be returning wrong values.
//    REG_WR32(NV_PRAMDAC_NVPLL_COEFF, DevinitCalcMNP(pDev, &clk));
    dacCalcMNP(pDev, Head, clk, &M, &N, &P);
    REG_WR32(NV_PRAMDAC_NVPLL_COEFF, DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _MDIV, M) |
             DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _NDIV, N) |
             DRF_NUM(_PRAMDAC, _NVPLL_COEFF, _PDIV, P));
                                        
/*
        ; Now, get the sequence table and the MCLK value
        ;
        mov     ax, cs:[di].MEM_INIT_VALS._MClk_PLL     ; Get MCLK value
        test    cs:[di].MEM_INIT_VALS._Flags, 01h        ; SDR or DDR?
        jz      @got_sdr2
        shl     ax,1                            ; Double MCLK on DDR
@got_sdr2:
        
        ; Here, ax = the decimal MCLK value
                
        push    bx      ; BX returns data from this call that we won't use
        call    CalcMNP                         ; Get MNP parameters
        pop     bx
        
        ;       AL = M parameter
        ;       AH = N parameter
        ;       DL = P parameter
        mov     esi,NV_PRAMDAC_MPLL_COEFF
        call    ProgPLL                         ; Program the PLL's
*/
    // program MCLK -- double the clock if flags indicate DDR memory
#ifdef FORCE_MCLOCKMHZ
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_WARNINGS, "NVRM: Nv10MemConfigureClocks: forcing MCLK Mhz to ", FORCE_MCLOCKMHZ);
    clk = FORCE_MCLOCKMHZ * 100;	// Clock MHz * 100
#else
    clk = memInitTable[memInitTableIndex].MClk_PLL;
#endif

    if (memInitTable[memInitTableIndex].Flags == DDR)
    {
        clk *= 2;
    }
    
    // use the RM kernel (dacCalcMFP()) to calculate M, N, and P values
    dacCalcMNP(pDev, Head, clk, &M, &N, &P);
    REG_WR32(
        NV_PRAMDAC_MPLL_COEFF, 
        DRF_NUM(_PRAMDAC, _MPLL_COEFF, _MDIV, M) |
            DRF_NUM(_PRAMDAC, _MPLL_COEFF, _NDIV, N) |
            DRF_NUM(_PRAMDAC, _MPLL_COEFF, _PDIV, P)
    );
    return rmStatus;
    
} // end of Nv10MemConfigureClocks()

    
RM_STATUS Nv10MemConfigureMemory
(
	PHWINFO pDev,
    MEM_INIT_TABLE_NV10 memInitTable, 
    U032 SDRSeqTable[], 
    U032 DDRSeqTable[]
)
{
	RM_STATUS rmStatus = RM_OK;
    U032* seqTable;
    U032 memInitTableIndex, initValue;
    U032 memInitDataIndex = 0; 
    U032 seqTableIndex = 0;
    
    U032 Head = 0;    
    
/*
        call    FullCpuOn               ; Turn on full CPU access
        
        ; Configure memory type by way of straps
        ;
        
        ; Get straps, and turn AL into an index to the table
        ;                       
        mov     al,NV_CIO_SCRATCH4_INDEX        ; CR3C
        call    ReadCRTCReg                     ; Read current value
;        shr    ah,4                            ; Move bits to AH[3:0]
;        shr    ax,8                            ; Move AH->AL
        shr     ax,12                           ; bits 0:3
        
;        mov     esi,NV_PEXTDEV_BOOT_0           ; Get strapping bits
;        NV_DOS_GET      esi
;        and     ax, NV_PEXTDEV_BOOT_0_STRAP_RAMCFG      ; Keep RAMCFG bits
;        shr     ax, RAMCFG_SHIFT
*/
    // get the index in the memory init table from scratch register 4
    CRTC_RD(NV_CIO_CRE_SCRATCH4__INDEX, memInitTableIndex, Head);
    memInitTableIndex >>= 4;
    
/*
        ; Size of config structure
        mov     cl, SIZEOF MEM_INIT_VALS        ; Get the size of the config struct
        mul     cl
        
        ; Now point at the right structure with CS:SI
        mov     di, offset bmp_MemInitTbl
        add     di,ax                           ; Add offset into init table
        
        mov     bx, offset SDR_Sequence_Table
        test    cs:[di].MEM_INIT_VALS._Flags, 01h        ; SDR or DDR?
        jz      @got_sdr

        ; We have DDR memory - turn on FBI_DIFFERENTIAL
        ; 8/25/99
        ;
        mov     esi,NV_PBUS_DEBUG_1
        NV_DOS_GET      esi                     ; Read register
        and     eax,NOT (NV_PBUS_DEBUG_1_FBI_DIFFERENTIAL)
        or      eax, NV_PBUS_DEBUG_1_FBI_DIFFERENTIAL_ENABLED
        NV_DOS_PUT      esi                     ; Re-write register
        
        mov     bx, offset DDR_Sequence_Table
@got_sdr: 
*/
    // set proper sequence table
    switch (memInitTable[memInitTableIndex].Flags)
    {
        case SDR:
            seqTable = SDRSeqTable;
            break;
            
        case DDR:
            seqTable = DDRSeqTable;
            break;
            
        default:
            return RM_ERROR;
    }
    
/*
        add     di,6                            ; Skip Flags, MCLK, NVCLK values
        
        ; CS:BX = pointer to sequence list
        ; CS:DI = pointer to sequence data (MEM_INIT_VALS)
@next_sequence:
        mov     esi, cs:[bx]                    ; Get Sequence
        cmp     esi, 0FFFFFFFFh                 ; End of sequence?
        je      @end_sequence
        
        cmp     esi,NV_PFB_PRE                  ; Precharge command?
        jne     @f  
        
        mov     eax, NV_PFB_PRE_CMD_PRECHARGE   ; Issue precharge command
        jmp     @set_pfb_reg
@@:
        cmp     esi,NV_PFB_PAD
        jne     @f            
        
        mov     eax, NV_PFB_PAD_CKE_NORMAL      ; Enable
        jmp     @set_pfb_reg
@@:
        cmp     esi,NV_PFB_REF                  ; Recharge command?
        jne     @f  
        
        mov     eax, NV_PFB_REF_CMD_REFRESH     ; Issue Refresh command
        jmp     @set_pfb_reg
@@:
        cmp     esi,NV_PFB_EMRS                 ; EMRS command?
        jne     @f  
        
        mov     eax, cs:[di]                    ; Read data value
        cmp     eax, 0FFFFFFFFh                 ; Is it a -1? (SDR)
        jne     @f                              ; Nope - program value
        
        add     di, 4                           ; Skip data value
        jmp     @skip_val                       ; Yep - skip the data value
@@:      
        mov     eax, cs:[di]                    ; Read data value
        add     di, 4                           ; Next data value
        
@set_pfb_reg:   
        NV_DOS_PUT      esi     
@skip_val:
        add     bx, 4                           ; Next sequence value
        jmp     @next_sequence
                
@end_sequence:
*/

    // execute the memory init sequence
    while (seqTable[seqTableIndex] != MEM_INIT_END_OF_SEQUENCE)
    {
        switch (seqTable[seqTableIndex])
        {
            case NV_PFB_PRE:
	            FLD_WR_DRF_DEF(_PFB,_PRE,_CMD,_PRECHARGE);
                break;
                
            case NV_PFB_PAD:
	            FLD_WR_DRF_DEF(_PFB,_PAD,_CKE,_NORMAL);
                break;
                
            case NV_PFB_REF:
	            FLD_WR_DRF_DEF(_PFB,_REF,_CMD,_REFRESH);
                break;
                
            case NV_PFB_EMRS:
                initValue = memInitTable[memInitTableIndex].data[memInitDataIndex++];
                if (initValue != 0xffffffff)
                {
                    REG_WR32(NV_PFB_EMRS, initValue);
                }
                break;
                
            default:
                REG_WR32(seqTable[seqTableIndex], memInitTable[memInitTableIndex].data[memInitDataIndex++]);
                break;
        }
        seqTableIndex++;
    }
    
    return rmStatus;
    
} // end of Nv10MemConfigureMemory()

RM_STATUS Nv10MemChipPreInit
(
	PHWINFO pDev
)
{
	RM_STATUS rmStatus = RM_OK;
    U016 crystalVal, ramcfgVal, data;
    U032 Head = 0;
/*
        mov     esi,NV_PEXTDEV_BOOT_0           ; Get strapping bits
        NV_DOS_GET      esi
        
        test    eax, NV_PEXTDEV_BOOT_0_STRAP_CRYSTAL_14318180
        jz      @freq_13
                 
        ; Bit set - we're on 14.318 reference frequency
        mov     ax,((NV_CIO_SCRATCH4_14MHZ SHL 8) OR (NV_CIO_SCRATCH4_INDEX))
        jmp     @f
        
@freq_13: 
        mov     ax,((NV_CIO_SCRATCH4_13MHZ SHL 8) OR (NV_CIO_SCRATCH4_INDEX))
@@:                                   
        call    WriteCRTCReg            ; Write the unlock   
        
        mov     esi,NV_PEXTDEV_BOOT_0           ; Get strapping bits
        NV_DOS_GET      esi
        and     ax, NV_PEXTDEV_BOOT_0_STRAP_RAMCFG ; Keep RAMCFG bits 5:2
        shl     ax, RAMCFG_SHIFT                ; Shift into bits 7:4
        mov     bh,al                           ; Put in BH a while
        mov     al,NV_CIO_SCRATCH4_INDEX        ; CR3C
        call    ReadCRTCReg                     ; Read current value
        or      ah,bh                           ; Or in new values
        call    WriteCRTCReg                    ; Write CR3C
*/
    // pack and save away the strapping values into scratch register 4
    crystalVal = (U016)REG_RD_DRF(_PEXTDEV,_BOOT_0,_STRAP_CRYSTAL);
    ramcfgVal = (U016)REG_RD_DRF(_PEXTDEV,_BOOT_0,_STRAP_RAMCFG);
    data = (U008)(ramcfgVal << 4 & 0x00f0) | (crystalVal & 0x000f);
    CRTC_WR(NV_CIO_CRE_SCRATCH4__INDEX, data, Head);

    return rmStatus;
    
    
} // end of Nv10MemChipPreInit()

RM_STATUS Nv10MemComputeMemory
(
	PHWINFO pDev
)
{
	RM_STATUS rmStatus = RM_OK;
	U032 testAddress;
    
/*
        call    FullCpuOn               ; Turn on full CPU access
        
        ; Ensure refresh is on
        mov     eax,NV_PFB_REFCTRL_VALID_1  ; NV_PFB_REFCTRL
        mov     esi,NV_PFB_REFCTRL
        NV_DOS_PUT      esi             ; Set Memory Config register
        
        ;
        ; Check for RAM width
        ;           
*/
    // ensure refresh is on
	FLD_WR_DRF_DEF(_PFB,_REFCTRL,_VALID,_1);
    
/*
        ; Write to Frame Buffer.
        mov     esi,(NV_PDFB OR NV_PDFB_ACCESS)
        add     si,1Ch                  ; Write to NV_PDFB+1Ch
        mov     eax,'NV10'              ; Write value to test.
        mov     edx, eax                ; Save write value
        
        NV_DOS_PUT      esi
        
        add     si,20h                  ; Write to NV_PDFB+3Ch
        xor     eax,eax                 ; Write 0's to drive pads low
                                                           
        NV_DOS_PUT      esi
        
        sub     si,20h                  ; Move back to offset 1Ch
        
        ; Read 2 bytes at 0MB + 0x1C (Lower 16 bits of 128-bit word)
        NV_DOS_GET      esi                            
        cmp     eax,edx                 ; Is EAX = 'NV5A'?            
                                                    
        je      @GoodBusSize            ; Yep - it's 128-bit bus
                                                
        ; Set PFB_CFG with correct BUS size                                   
        mov     esi,NV_PFB_CFG
        NV_DOS_GET      esi             ; Get Memory Config register
        and     eax,0FFFFFFEFh          ; Clear RAM width bit to make 64-bit
        NV_DOS_PUT                      ; Write it back out
        
@GoodBusSize:                         
*/
    // check for RAM width as follows:
    //   1.  write non-zero test word to 0x0000001c
    //   2.  write out a zero word to 0x0000003c
    //   3.  attempt to read lower word at 0x0000001c
    //   4.  if not successful, change bus width to 64 bits
    //   5.  otherwise, bus width remains at default of 128 bits
    FB_WR32(0x0000001c, 'NV10');
    FB_WR32(0x0000003c, 0x00000000);
    if (FB_RD16(0x0000001c) != '10')
    {
        FLD_WR_DRF_DEF(_PFB,_CFG,_BUS,_64);
        //Nv05MemLatchMemConfig(pDev);
    }
    
/*
        ;
        ; Get memory size now
        ;
        
        ; Default value in PFB_CFG is largest memory buffer supported
        ;
        mov     esi,NV_PFB_CSTATUS
        NV_DOS_GET      esi             ; Get Memory Size register
        
        ; EAX now has the largest memory size supported, as calc'd by the HW
        shr     eax,1                   ; Make next size down - this will
                                        ; force a write at the MB just above
                                        ; the next smaller size.
        mov     esi,(NV_PDFB OR NV_PDFB_ACCESS)
        add     esi,eax                 ; Add in max size
                           
        mov     eax,'NVDA'              ; Write value to test.
        mov     edx, eax                ; Save write value
                           
        NV_DOS_PUT      esi             ; Write test value to memory
        add     si,20h                  ; Write another address w/diff data
        xor     eax,eax
        NV_DOS_PUT      esi             ; Force pad's low
        sub     si,20h                  ; Return to original address
        NV_DOS_GET      esi             ; Read it back        

        cmp     eax,edx                 ; Read back our original write value?
        je      @Exit                   ; Correct memory size is already set.
                                                              
        mov     esi,NV_PFB_CFG
        NV_DOS_GET      esi             ; Get Memory Config register
                                                     
        and     ah,0EFh                 ; Clear EXT_BANK to 1 bank
                                                                
        NV_DOS_PUT      esi             ; Set Memory Config register
@Exit:
*/    
    // check for RAM width as follows:
    //   1.  get what the HW thinks is the largest memory size supported
    //   2.  write test word to 1 byte beyond half the largest
    //   3.  write zero word to 16 bytes beyond half 
    //   4.  attempt to read the original test word
    //   5.  if successful, memory size is already correct;
    //       otherwise, adjust memory size
    testAddress = REG_RD32(NV_PFB_CSTATUS) / 2;
    FB_WR32(testAddress, 'NVDA');
    FB_WR32(testAddress + 0x10, 0x00000000);
    if (FB_RD32(testAddress) != 'NVDA')
    {
        FLD_WR_DRF_DEF(_PFB,_CFG,_EXTBANK,_0);
    }
    
	return rmStatus;

} // end of Nv10MemComputeMemory()


// end of nv10mem.c

