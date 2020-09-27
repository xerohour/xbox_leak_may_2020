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
*	Module: nv05mem.c
*
*	Description:
*		Device-specific memory sizing code, for NV5.
*
*	Revision History:
*		Original -- 5/99 Jeff Westerinen
*
**************************************************************************************************************/

#ifdef NV_PORTABLE

/* "portable" form for use outside resman and by external source customers */
/* look in devinit/portable/ for more details */
#include <nv_portable.h>
#include <nv_ref.h>
#include <nv4_ref.h>
#include <devinit.h>

#else	/* not NV_PORTABLE --> Nvidia internal */

#include <nvrm.h>
#include <nvhw.h>
#include <nv4_ref.h>
#include <os.h>
#include <devinit.h>

#endif

extern bmpMemInitData Nv05_bmp_MemInitTbl[];
extern U032 Nv05_bmp_ScrambleTbl[];

RM_STATUS Nv05MemResizeMemory
(
    PHWINFO pDev
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 i, data;

	// TO DO: is this necessary?
    // turn on full CPU access
	//...
     
    /*
        ; For UMA mode, we don't configure the memory
        ;
        mov     esi,NV_PFB_BOOT_0
        NV_DOS_GET      esi             ; Get Memory Config register
        test    ah,01h                  ; Is bit 8 set?
        jnz     @Exit                   ; Yep - UMA is on! Nada to configure!
        ;
        ; For UMA mode above.
        ;
    */
    // only size memory if UMA is disabled
    if (REG_RD_DRF(_PFB,_BOOT_0,_UMA) == NV_PFB_BOOT_0_UMA_DISABLE)
    {
	    // enable refresh for memory sizing
        FLD_WR_DRF_DEF(_PFB,_DEBUG_0,_REFRESH,_ENABLED);

        // TO DO: remove this when KevinM says OK
        // scramble the RAM from the table
        for (i = 0; i < 8; i++)
        {
            REG_WR32(NV_PFB_SCRAMBLE(i), Nv05_bmp_ScrambleTbl[i]);
        }
    
        // configure memory type by way of straps as follows:
        //   1.  get the strapping value from NV_PEXTDEV_BOOT_0
        //   2.  use that value as an index into the memory init table
        //   3.  the first member of the selected table element configures NV_PFB_BOOT_0
        //   4.  the second member of the selected table element configures PFB_CONFIG_1
        i = REG_RD_DRF(_PEXTDEV,_BOOT_0,_STRAP_RAMCFG);
        data = (REG_RD32(NV_PFB_BOOT_0) & 0xFFFFFFC0) | Nv05_bmp_MemInitTbl[i].PFB_BOOT_0_value;
        REG_WR32(NV_PFB_BOOT_0, data);
    
        // scramble the RAM, if indicated
    /*    
            ; Here, look for need to scramble RAM
            test    cl,SCRAMBLE_RAM                 ; Bit says to scramble the RAM?
            jz      @no_scramble
    
            ; Scramble the RAM according to the RAM table defined
            ; earlier.
            ;
            mov     di, offset bmp_ScrambleTbl      ; Point at table of RAM values
            mov     esi,NV_PFB_SCRAMBLE_RAM_0       ; Get strapping bits
            mov     cx,8                            ; Do 8 locations
    @@: 
            mov     eax,cs:[di]                     ; Read value 1
            NV_DOS_PUT      esi                     ; Write it out
            add     di,4                            ; Next DWORD
            add     esi,4                           ; Next DWORD
            loop    @b        
                                  ; and loop again
            ; Enable the scramble
            mov     esi,NV_PFB_CONFIG_0             ; Access Scramble RAM
            NV_DOS_GET      esi        
            or      eax, NV_PFB_CONFIG_0_SCRAMBLE_ACTIVE
            NV_DOS_PUT      esi        
                   
    @no_scramble:
    */
        if (Nv05_bmp_MemInitTbl[i].PFB_CONFIG_1_value == SCRAMBLE_RAM)
        {
    // TO DO: enable this code when Kevin M says OK
    #if 0
            for (i = 0; i < 8; i++)
            {
                REG_WR32(NV_PFB_SCRAMBLE_RAM_0 + (i * 4), Nv05_bmp_ScrambleTbl[i]);
            }
    #endif    
            FLD_WR_DRF_DEF(_PFB,_CONFIG_0,_SCRAMBLE,_ACTIVE);
        }
      
        // toggle latency bit to latch in the config, as follows:
        //   1. Clear to CL=2 and the SPECIAL32 bit or default SPECIAL32 value
        //   2. Set back to CL=3 
    /*
            pop     ecx                             ; Get flags back
            and     cl,SPECIAL_32                   ; Keep only SPECIAL_32 bit
            shl     ecx,20                          ; put in proper bit location
        
            ; Toggle PFB_CONFIG_1 CAS latency bit to generate MRS cycle.
            mov     esi,NV_PFB_CONFIG_1             ; Get Config1 bits
            NV_DOS_GET      esi
        
            and     eax,0FF8FFFFEh                  ; Clear to CL=2 and the SPECIAL32 bit
            or      eax,ecx                         ; Or default SPECIAL32 value in.
            NV_DOS_PUT      esi                     ; Write it back out
        
            or      al,01h                          ; Set back to CL=3
            NV_DOS_PUT      esi                     ; Write it back out
    */
        data = (REG_RD32(NV_PFB_CONFIG_1) & 0xFF8FFFFE) | ((Nv05_bmp_MemInitTbl[i].PFB_CONFIG_1_value & SPECIAL_32) << 20);
        REG_WR32(NV_PFB_CONFIG_1, data);
        data |= 0x00000001;
        REG_WR32(NV_PFB_CONFIG_1, data);
    
        // check for RAM width as follows:
        //   1.  assume width is 128 bits
        //   2.  write out a 128 bit test data word
        //   3.  attempt to read lower word
        //   4.  if not successful, change bus width to 64 bits
        //   5.  otherwise, bus width remains at default of 128 bits
        for (i = 0; i < 4; i++)
        {
            FB_WR32(i * 4, 'NV5A');
        }    
        if (FB_RD16(0x0000000C) != '5A')
        {
            FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_WIDTH_128,_OFF);
        }
    
        // determine the memory size -- default amount is 32Meg
        if (REG_RD_DRF(_PFB,_BOOT_0,_RAM_AMOUNT) == NV_PFB_BOOT_0_RAM_AMOUNT_32MB)
        {
            // write at 16Meg+1 to determine 32 or 16 Meg
            FB_WR32(0x01000000, 'NV5B');
            if (FB_RD32(0x01000000) == 'NV5B')
            {
                // read at 0 to determine if it wrapped on VANTA
                FB_WR32(0x00000000, 'A5VN');
                if (FB_RD32(0x00000000) != 'A5VN')
                {
                    // 16MB detected, so change config
                    FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_AMOUNT,_16MB);
                }
            }
            else
            {
                // 16MB detected, so change config
                FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_AMOUNT,_16MB);
            }
        }
        else
        {
            // test for 16 Meg
            if (REG_RD_DRF(_PFB,_BOOT_0,_RAM_AMOUNT) == NV_PFB_BOOT_0_RAM_AMOUNT_16MB)
            {
                // write at 8Meg+1 to determine 16Meg or less
                FB_WR32(0x00800000, 'NV5C');
                if (FB_RD32(0x00800000) != 'NV5C')
                {
                    // write at 4Meg+1 to determine 8 or 4 Meg
                    FB_WR32(0x00400000, 'NV5D');
                    if (FB_RD32(0x00400000) == 'NV5D')
                    {
                        // 8MB detected, so change config
                        FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_AMOUNT,_8MB);
                    }
                    else
                    {
                        // 4MB detected, so change config
                        FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_AMOUNT,_4MB);
                    }
                }
            }
            else
            {
                // write at 4Meg+1 to determine 8 or 4 Meg
                FB_WR32(0x00400000, 'NV5D');
                if (FB_RD32(0x00400000) == 'NV5D')
                {
                    // 8MB detected, so change config
                    FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_AMOUNT,_8MB);
                }
                else
                {
                    // 4MB detected, so change config
                    FLD_WR_DRF_DEF(_PFB,_BOOT_0,_RAM_AMOUNT,_4MB);
                }
            }
        }
    }    
    
	// TO DO: is this necessary?
	// turn off full CPU access
	//...

	return rmStatus;

} // end of DevinitResizeSgram()

VOID Nv05MemLatchMemConfig
(
    PHWINFO pDev
)
{
    // save the original CAS latency
    U032 latency = REG_RD_DRF(_PFB,_CONFIG_1,_CAS_LATENCY);
    
    // toggle the LSB 1-0-1
    FLD_WR_DRF_DEF(_PFB,_CONFIG_1,_CAS_LATENCY,_3);
    FLD_WR_DRF_DEF(_PFB,_CONFIG_1,_CAS_LATENCY,_2);
    FLD_WR_DRF_DEF(_PFB,_CONFIG_1,_CAS_LATENCY,_3);
    
    // restore original CAS latency
    FLD_WR_DRF_NUM(_PFB,_CONFIG_1,_CAS_LATENCY,latency);
    
} // end of Nv04MemLatchMemConfig()


// end of nv04mem.c


// end of nv05mem.c

