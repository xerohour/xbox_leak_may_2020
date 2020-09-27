;++
;
;  Copyright (c) Microsoft Corporation. All rights reserved
;
;  Module Name:
;
;    nv2ainit.inc
;
;  Abstract:
;
;    NV2A initialization table
;
;    This module contains table of commands used by South Bridge to initialize
;    hardware and test memory during boot startup.  For definition of each
;    command, please see initcode.inc and command.inc
;--

page    84,132
.MODEL compact
.486p

INCLUDE COMMAND.INC
INCLUDE NV2A.INC

CODE SEGMENT PARA PUBLIC 'code'
  assume  ds:code,es:nothing,ss:nothing
org 0000h

;
; NB data for NV2A on the DVT boards
;

;
; Not currently documented, NV2A expects table ptrs to exist in the first
; 2 DWORDS of the ROM image, which point to the beginning of the ROM table.
; The MCPX still expects to find it's init values starting at 0x70.
;
dd NV2A_XBOX_ROM_TABLE_PTR + 1        ; LSB needs to be 1 to appear valid
dd NV2A_XBOX_ROM_TABLE_PTR

; CR_ROM_NB_BOOT_HEADER                    (0*32+31):(0*32+0) /* R--VF */
;
dd 2B16D065h

; CR_ROM_NB_BOOT_PROC                       (1*32+0):(1*32+0) /* R--VF */
; CR_ROM_NB_BOOT_PROC_INTEL                        0x00000001 /* R---V */
; CR_ROM_NB_BOOT_RAMTYPE                    (1*32+2):(1*32+2) /* R--VF */
; CR_ROM_NB_BOOT_RAMTYPE_DDR                       0x00000001 /* R---V */
; CR_ROM_NB_BOOT_RAMWIDTH                   (1*32+3):(1*32+3) /* R--VF */
; CR_ROM_NB_BOOT_RAMWIDTH_128                      0x00000001 /* R---V */
; CR_ROM_NB_BOOT_MEM_POSTDIV                (1*32+7):(1*32+4) /* R--VF */
; CR_ROM_NB_BOOT_AGP_POSTDIV               (1*32+11):(1*32+8) /* R--VF */
; CR_ROM_NB_BOOT_FSB_ROM                  (1*32+15):(1*32+12) /* R--VF */
; CR_ROM_NB_BOOT_FSB_66                   (1*32+19):(1*32+16) /* R--VF */
; CR_ROM_NB_BOOT_FSB_100                  (1*32+23):(1*32+20) /* R--VF */
; CR_ROM_NB_BOOT_FSB_TUALATIN             (1*32+27):(1*32+24) /* R--VF */
; CR_ROM_NB_BOOT_FSB_133                  (1*32+31):(1*32+28) /* R--VF */
;
; dd 4444444dh    ; (100MHz FSB hardcoded)
; dd 3346323Dh      ; (AGP_PDIV = 2, MEM_PDIV = 3), start at 133MHz
dd 3346322Dh    ; (AGP_PDIV = 2, MEM_PDIV = 2)

;    These COREPLL values are based on using a 13.5MHz xtal.
;
;    NOTE: If you change the MDIV, NDIV, FSB POSTDIV or MEM_POSTDIV you'll
;    need to sync the change with the  CR_CPU_MPLL_COEFF write in the inittbl
;    below.
;
; CR_ROM_NB_BOOT_COREPLL_MDIV_66            (2*32+7):(2*32+0) /* R--VF */
; CR_ROM_NB_BOOT_COREPLL_MDIV_66_VALUE             0x00000007 /* R---V */ 
; CR_ROM_NB_BOOT_COREPLL_MDIV_100          (2*32+15):(2*32+8) /* R--VF */
; CR_ROM_NB_BOOT_COREPLL_MDIV_100_VALUE            0x00000007 /* R---V */ 
; CR_ROM_NB_BOOT_COREPLL_MDIV_TUALATIN    (2*32+23):(2*32+16) /* R--VF */
; CR_ROM_NB_BOOT_COREPLL_MDIV_TUALATIN_VALUE       0x00000007 /* R---V */ 
; CR_ROM_NB_BOOT_COREPLL_MDIV_133         (2*32+31):(2*32+24) /* R--VF */
; CR_ROM_NB_BOOT_COREPLL_MDIV_133_VALUE            0x00000007 /* R---V */ 
;
; dd 07070707h
; dd 01010101h
dd 01010101h

; CR_ROM_NB_BOOT_COREPLL_NDIV_66            (3*32+7):(3*32+0) /* R--VF */
; CR_ROM_NB_BOOT_COREPLL_NDIV_66_VALUE             0x00000022 /* R---V */
; CR_ROM_NB_BOOT_COREPLL_NDIV_100          (3*32+15):(3*32+8) /* R--VF */
; CR_ROM_NB_BOOT_COREPLL_NDIV_100_VALUE            0x00000034 /* R---V */
; CR_ROM_NB_BOOT_COREPLL_NDIV_TUALATIN    (3*32+23):(3*32+16) /* R--VF */
; CR_ROM_NB_BOOT_COREPLL_NDIV_TUALATIN_VALUE       0x00000045 /* R---V */
; CR_ROM_NB_BOOT_COREPLL_NDIV_133         (3*32+31):(3*32+24) /* R--VF */
; CR_ROM_NB_BOOT_COREPLL_NDIV_133_VALUE            0x00000045 /* R---V */
;
; dd 45453422h
; dd 07070707h        ; (100MHz FSB)
; dd 08080808h        ; (106MHz FSB)
; dd 0A0A0A0Ah        ; (135MHz FSB)
; dd 09090909h        ; (133MHz FSB)
dd 08080808h          ; (133MHz FSB 16.66MHz input clock)

; CR_ROM_NB_BOOT_COREPLL_MDIV_ROM           (4*32+7):(4*32+0) /* R--VF */
; CR_ROM_NB_BOOT_COREPLL_MDIV_ROM_VALUE            0x00000007 /* R---V */
; CR_ROM_NB_BOOT_COREPLL_NDIV_ROM          (4*32+15):(4*32+8) /* R--VF */
; CR_ROM_NB_BOOT_COREPLL_NDIV_ROM_VALUE            0x00000045 /* R---V */
; CR_ROM_NB_BOOT_ASRC_CPU                 (4*32+23):(4*32+16) /* R--VF */
; CR_ROM_NB_BOOT_ASRC_LDT                 (4*32+31):(4*32+24) /* R--VF */
;
; dd 4507h
; dd 0801h          ; (106MHz FSB)
; dd 0701h          ; (100MHz FSB)
; dd 0A01h          ; (135MHz FSB)
; dd 1D03h          ; (133MHz FSB)
dd 0801h            ; (133MHz FSB w/ 16.66MHz input clock)

; CR_ROM_NB_BOOT_SADDOUTCLK_DLY             (5*32+3):(5*32+0) /* R--VF */
; CR_ROM_NB_BOOT_SDATAOUTCLK_DLY            (5*32+7):(5*32+4) /* R--VF */
; CR_ROM_NB_BOOT_CPU_VREF_EN_               (5*32+8):(5*32+8) /* R--VF */
; CR_ROM_NB_BOOT_APIC_33                    (5*32+9):(5*32+9) /* R--VF */
; CR_ROM_NB_BOOT_COREPLL_SETUP            (5*32+18):(5*32+10) /* R--VF */
; CR_ROM_NB_BOOT_CPUPLL_SETUP             (5*32+27):(5*32+19) /* R--VF */
; CR_ROM_NB_BOOT_COREPLL_DLY              (5*32+31):(5*32+28) /* R--VF */
;
dd 0C8FC7C8Ah

; CR_ROM_NB_BOOT_CTRIM_CPUCLKFB             (6*32+3):(6*32+0) /* R--VF */
; CR_ROM_NB_BOOT_CTRIM_CPUCLKFB_VALUE              0x00000003 /* R---V */
; CR_ROM_NB_BOOT_CTRIM_CPUCLK2X_BL          (6*32+7):(6*32+4) /* R--VF */
; CR_ROM_NB_BOOT_CTRIM_CPUCLK2X_BL_VALUE           0x00000001 /* R---V */
; CR_ROM_NB_BOOT_CTRIM_CPUCLK2X_BR         (6*32+11):(6*32+8) /* R--VF */
; CR_ROM_NB_BOOT_CTRIM_CPUCLK2X_BR_VALUE           0x00000002 /* R---V */
; CR_ROM_NB_BOOT_CTRIM_CPUCLK_NW          (6*32+15):(6*32+12) /* R--VF */
; CR_ROM_NB_BOOT_CTRIM_CPUCLK_NW_VALUE_DONTCARE    0x00000000 /* R---V */
; CR_ROM_NB_BOOT_CTRIM_CPUCLK_SW          (6*32+19):(6*32+16) /* R--VF */
; CR_ROM_NB_BOOT_CTRIM_CPUCLK_SW_VALUE             0x00000009 /* R---V */
; CR_ROM_NB_BOOT_CTRIM_CPUCLK_SE          (6*32+23):(6*32+20) /* R--VF */
; CR_ROM_NB_BOOT_CTRIM_CPUCLK_SE_VALUE             0x00000002 /* R---V */
; CR_ROM_NB_BOOT_CTRIM_CPUOCLK_BL         (6*32+27):(6*32+24) /* R--VF */
; CR_ROM_NB_BOOT_CTRIM_CPUOCLK_BL_VALUE            0x00000002 /* R---V */
; CR_ROM_NB_BOOT_CTRIM_CPUOCLK_BR         (6*32+31):(6*32+28) /* R--VF */
; CR_ROM_NB_BOOT_CTRIM_CPUOCLK_BR_VALUE            0x00000002 /* R---V */
;
dd 44290213h        ; A03


; CR_ROM_NB_BOOT_CTRIM_CPUCLKOUT            (7*32+3):(7*32+0) /* R--VF */
; CR_ROM_NB_BOOT_CTRIM_CPUCLKOUT_VALUE             0x00000008 /* R---V */
; CR_ROM_NB_BOOT_CTRIM_APICCLK_MCP          (7*32+7):(7*32+4) /* R--VF */
; CR_ROM_NB_BOOT_CTRIM_APICCLK_MCP_VALUE           0x00000009 /* R---V */
; CR_ROM_NB_BOOT_CTRIM_APICCLK_CPU         (7*32+11):(7*32+8) /* R--VF */
; CR_ROM_NB_BOOT_CTRIM_APICCLK_CPU_VALUE           0x00000009 /* R---V */
; CR_ROM_NB_BOOT_CTRIM_COREPLLFB          (7*32+15):(7*32+12) /* R--VF */
; CR_ROM_NB_BOOT_CTRIM_COREPLLFB_VALUE             0X00000004 /* R---V */
; CR_ROM_NB_BOOT_CTRIM_CPUPLLFB           (7*32+19):(7*32+16) /* R--VF */
; CR_ROM_NB_BOOT_CTRIM_SPARE1             (7*32+23):(7*32+20) /* R--VF */
; CR_ROM_NB_BOOT_CTRIM_SPARE1_VALUE_DONTCARE       0x00000000 /* R---V */
; CR_ROM_NB_BOOT_CTRIM_SPARE2             (7*32+27):(7*32+24) /* R--VF */
; CR_ROM_NB_BOOT_CTRIM_SPARE2_VALUE_DONTCARE       0x00000000 /* R---V */
; CR_ROM_NB_BOOT_CTRIM_SPARE3             (7*32+31):(7*32+28) /* R--VF */
;
IFDEF SYS_DVT6
dd 90004998h
ELSE
dd 00064998h
ENDIF

; CR_ROM_NB_BOOT_CSHAPE_CPUCLKFB            (8*32+3):(8*32+0) /* R--VF */
; CR_ROM_NB_BOOT_CSHAPE_CPUCLKFB_VALUE             0x00000000 /* R---V */
; CR_ROM_NB_BOOT_CSHAPE_CPUCLK2X            (8*32+7):(8*32+4) /* R--VF */
; CR_ROM_NB_BOOT_CSHAPE_CPUCLK2X_VALUE             0x00000000 /* R---V */
; CR_ROM_NB_BOOT_CSHAPE_CPUCLK             (8*32+11):(8*32+8) /* R--VF */
; CR_ROM_NB_BOOT_CSHAPE_CPUCLK_VALUE               0x00000000 /* R---V */
; CR_ROM_NB_BOOT_CSHAPE_CPUCLKOUT         (8*32+15):(8*32+12) /* R--VF */
; CR_ROM_NB_BOOT_CSHAPE_CPUCLKOUT_VALUE            0x00000000 /* R---V */
; CR_ROM_NB_BOOT_CSHAPE_APICCLK_CPU       (8*32+19):(8*32+16) /* R--VF */
; CR_ROM_NB_BOOT_CSHAPE_APICCLK_CPU_VALUE          0x00000000 /* R---V */
; CR_ROM_NB_BOOT_CSHAPE_APICCLK_MCP       (8*32+23):(8*32+20) /* R--VF */
; CR_ROM_NB_BOOT_CSHAPE_APICCLK_MCP_VALUE          0x00000000 /* R---V */
; CR_ROM_NB_BOOT_CSHAPE_SPARE1            (8*32+27):(8*32+24) /* R--VF */
; CR_ROM_NB_BOOT_CSHAPE_SPARE1_VALUE_DONTCARE      0x00000000 /* R---V */
; CR_ROM_NB_BOOT_CSHAPE_SPARE2            (8*32+31):(8*32+28) /* R--VF */
; CR_ROM_NB_BOOT_CSHAPE_SPARE2_VALUE               0x00000000 /* R---V */
;
dd 000000000h

; CR_ROM_NB_BOOT_INTEL_BPRI_                (9*32+0):(9*32+0) /* R--VF */    
; CR_ROM_NB_BOOT_INTEL_BNR_                 (9*32+1):(9*32+1) /* R--VF */    
; CR_ROM_NB_BOOT_INTEL_ADS_                 (9*32+2):(9*32+2) /* R--VF */    
; CR_ROM_NB_BOOT_INTEL_ADDR_               (9*32+31):(9*32+3) /* R--VF */    
;
dd 0FFFFFFFFh

; CR_ROM_NB_BOOT_INTEL_TRDY_              (10*32+0):(10*32+0) /* R--VF */
; CR_ROM_NB_BOOT_INTEL_RS_                (10*32+3):(10*32+1) /* R--VF */
; CR_ROM_NB_BOOT_INTEL_REQ_               (10*32+8):(10*32+4) /* R--VF */
; CR_ROM_NB_BOOT_INTEL_HITM_              (10*32+9):(10*32+9) /* R--VF */
; CR_ROM_NB_BOOT_INTEL_HIT_             (10*32+10):(10*32+10) /* R--VF */
; CR_ROM_NB_BOOT_INTEL_DRDY_            (10*32+11):(10*32+11) /* R--VF */
; CR_ROM_NB_BOOT_INTEL_DEFER_           (10*32+12):(10*32+12) /* R--VF */
; CR_ROM_NB_BOOT_INTEL_DBSY_            (10*32+13):(10*32+13) /* R--VF */
; CR_ROM_NB_BOOT_INTEL_AGTL_PLUS        (10*32+14):(10*32+14) /* R--VF */
; CR_ROM_NB_BOOT_INTEL_SPARE1           (10*32+31):(10*32+15) /* R--VF */
;
dd 0FFFFFFFFh

;
; 14 DWORDS used (11 for CR_ROM_BOOT, 2 table ptrs, 1 DWORD ROM rev)
;
dd (28-14) dup (0h)

; ROM/Trims Revision number
;
; To keep track of TRIM changes and other inittbl/datatbl changes, we'll
; use this DWORD as the revision ID. We'll also keep the history here, so
; clear what's been changed.
;
; rev1.00 - 
;    . set QUSE_NORMAL in NV_PBUS_FBIO_CFG
;    . set NB_BOOT_CPUPLL_SETUP[3:0] and NB_BOOT_COREPLL_SETUP[3:0] to 0xF
;    . set NB_BOOT_CTRIM_CPUPLLFB to 0x6
;    . set NV_PBUS_DEBUG_CTRIM_6 to 0x66660000 and
;          NV_PBUS_DEBUG_CTRIM_2 to 0x11110000, in that order
;    . clear most significant nibble from NV_PBUS_DEBUG_CTRIM_4
;    . add SYS_DVT3 ifdef for (DVT3) 13.5 vs. (DVT4) 16.6 input clocks,
;      with changes to:
;          NB_BOOT_COREPLL_NDIV and MDIV
;          NV_PRAMDAC_NVPLL_COEFF
;          CR_CPU_MPLL_COEFF
;    . add NV2A_REV_A03 ifdef for A03 related trims:
;          CTRIM_CPUOCLK_BL = 0x4
;          CTRIM_CPUOCLK_BR = 0x4
;          CR_CPU_CST_0     = 0xF0F0C0C0
;    . update 13.5 and 16.6 calibration speedometer settings
;          (previous VCC values were incorrect for both input clocks)
;
; rev1.01 - 
;    . set WBC from 0x11000001 (which is no buffering) to 0x11000016
;    . reset PFB_ARB_XFER_SZ_ZW from 0xF (unlimited burst) to 0x8
;          since NV2A doesn't need the zcull workaround like NV20.
;    . set DQS/DQ inbound delay to 0x1/0x0 in the Samsung calibration table
;    . set the MCLK drive/slew parameters to their max (helps clock jitter).
;
; rev1.02 -
;    . change NV_PBUS_FBIO_CFG (from 0x1 to 0x10) for 1/4 clock delay
;    . change NV_PBUS_DEBUG_CTRIM_2 (from 0x11110000 to 0x0)
;
; rev1.03 - 
;    . max MCP_LEG_CFG_45_PCICLKO5_TRIM to better line up the PCI clk
;           for the LPC card 
;    . DVT6 trims (tested with the Cypress zero delay buffer):
;           PBUS_DEBUG_CTRIM_2 = 0xFFFF0000
;           CR_ROM_NB_BOOT_CTRIM_CPUPLLFB = 0x0
;           CR_ROM_NB_BOOT_CTRIM_SPARE3   = 0x9
;           change calibration inbound delay values (DQS = 0, DATA = 1)
;
; rev1.04 - 
;    . Add support for MCP C03's different base address
;
; rev1.05 - 
;    . update Micron FBIO_DLY and ADDRESS_DRIVE settings during init and
;        in the calibration table.
;    . PBUS_DEBUG_CTRIM_2 is now different between Samsung/Micron.
;
; rev1.06 - 
;    . Changed pre-calibration defaults for Samsung memory.
;    . FBIO_DLY default was 012121212 is now 009090909
;    . CLKDRV/CLKSLW value is now 0FFFFFFFF
;

;
dd 00000106h        ; bits 31:8 = major rev, 7:0 = minor rev

;
; SB data for MCPX on the DVT boards (starts at offset 0x70)
;

; MCP_ROM_BOOT_FREQ_STRAP                 (28*32+3):(28*32+0) /* R--VF */
; MCP_ROM_BOOT_FREQ_STRAP_SAFE                     0x0000000f /* R---V */
;
;dd 00000000h
dd 0Fh

; MCP_ROM_BOOT_PIC_DIS                    (29*32+0):(29*32+0) /* RWIVF */
; MCP_ROM_BOOT_PIT_DIS                    (29*32+1):(29*32+1) /* RW-VF */
; MCP_ROM_BOOT_DMA_DIS                    (29*32+2):(29*32+2) /* RW-VF */
; MCP_ROM_BOOT_P92_DIS                    (29*32+3):(29*32+3) /* RW-VF */
; MCP_ROM_BOOT_FERR_DIS                   (29*32+4):(29*32+4) /* RW-VF */
; MCP_ROM_BOOT_RTC_NMI_DIS                (29*32+5):(29*32+5) /* RW-VF */
; MCP_ROM_BOOT_USB0_DIS                   (29*32+8):(29*32+8) /* RW-VF */
; MCP_ROM_BOOT_USB1_DIS                   (29*32+9):(29*32+9) /* RW-VF */
; MCP_ROM_BOOT_MAC0_DIS                 (29*32+10):(29*32+10) /* RW-VF */
; MCP_ROM_BOOT_MAC1_DIS                 (29*32+11):(29*32+11) /* RW-VF */
; MCP_ROM_BOOT_ACI_DIS                  (29*32+12):(29*32+12) /* RW-VF */
; MCP_ROM_BOOT_MCI_DIS                  (29*32+13):(29*32+13) /* RW-VF */
; MCP_ROM_BOOT_IDE_DIS                  (29*32+14):(29*32+14) /* RW-VF */
; MCP_ROM_BOOT_P2P_DIS                  (29*32+15):(29*32+15) /* RW-VF */
; MCP_ROM_BOOT_SMB_DIS                  (29*32+16):(29*32+16) /* RW-VF */
; MCP_ROM_BOOT_FST_RST                  (29*32+24):(29*32+24) /* R--VF */
; MCP_ROM_BOOT_FST_RST_NO                          0x00000000 /* R---V */
; MCP_ROM_BOOT_REBOOT                   (29*32+25):(29*32+25) /* RW-VF */
; MCP_ROM_BOOT_REBOOT_NO                           0x00000000 /* RW--V */
; MCP_ROM_BOOT_LDT_FRQ                  (29*32+26):(29*32+26) /* RW-VF */
; MCP_ROM_BOOT_LDT_FRQ_400MHz                      0x00000000 /* RW--V */
; MCP_ROM_BOOT_LDT_FRQ_200MHz                      0x00000001 /* RW--V */
; MCP_ROM_BOOT_PROC                     (29*32+27):(29*32+27) /* RW-VF */
; MCP_ROM_BOOT_PROC_INTEL                          0x00000000 /* RWI-V */
; MCP_ROM_BOOT_APU_TYP                  (29*32+28):(29*32+28) /* RW-VF */
; MCP_ROM_BOOT_APU_TYP_256V                        0x00000000 /* RW--V */
; MCP_ROM_BOOT_IDE_SEC                  (29*32+29):(29*32+29) /* RW-VF */
; MCP_ROM_BOOT_IDE_SEC_IDE                         0x00000000 /* RWI-V */
; MCP_ROM_BOOT_XBOX_ROM_SPEED           (29*32+30):(29*32+30) /* RWIVF */
; MCP_ROM_BOOT_XBOX_ROM_SPEED_SLOW                 0x00000000 /* RWI-V */
; MCP_ROM_BOOT_XBOX_ROM_SPEED_FAST                 0x00000001 /* RW--V */
;
IFDEF SYS_RETAIL
dd 40004400h
ELSE
dd 40000000h
ENDIF
;
; Init table identifier number 
;
IFDEF SYS_DVT4
dd 000000046h            
ELSE
IFDEF SYS_QT
dd 000000070h
ELSE
dd 000000060h
ENDIF
ENDIF
;
; offset from ROM base to the datatbl
;
; IMPORTANT: If size of the init table code below changes, this offset must be adjusted
;
dd DATATBL_BASE_OFFSET   

;
; Force 128 byte alignment MCPX in XBox mode.
;
org 0080h

;//////////////////////////////////////////
;
; NV2A init sequence.
;
;//////////////////////////////////////////

IFDEF SYS_RETAIL
nv_command_struct <COMMAND_UNUSED1, 000081000h, 001h>   
nv_command_struct <COMMAND_UNUSED2, 0000114h, 0228h>   
ENDIF

        
;
; Setup the IO BAR in MCP.  MCP_LEG_CFG_4 below evaluates to different
; values based on MCP rev we are building for. 
;
nv_command_struct<COMMAND_PCI_CFG_WRITE, MCP_LEG_CFG_4, 8001h>  

;
; Enable the IO space
;
nv_command_struct <COMMAND_PCI_CFG_WRITE, MCP_LEG_CFG_1, 3h>          

;
; Disable the TCO timer
;
nv_command_struct <COMMAND_OUT_BYTE,      08049h,     8h> 

;
; Also, the KBDRSTIN# signal should've been pulled low on XBox. It wasn't and
; was left floating. As a workaround, will change the mode to be a GPIO.
;
nv_command_struct <COMMAND_OUT_BYTE,      080D9h,     0h>       ; KBDRSTIN# in GPIO mode
nv_command_struct <COMMAND_OUT_BYTE,      08026h,     1h>       ; disable PWRBTN# in PM26

;
; First, we have to gain access to the integrated NV20. This involves
; enabling intergrated graphics and setting up the NB P2P to configure
; Bus2.
;
nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_APC_P2P_19, CR_APC_P2P_19_NV20_ENABLE>    ; internal graphics enabled
nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_APC_P2P_6,  CR_APC_P2P_6_SUB_SEC_PRI_BUS> ; setup secondary bus 1

;
; Now, config cycles to graphics should work. Setup the memory base/limit
; on the P2P and the BAR0 of the integrated graphics, so we can access priv
; regs. Enable BUS_MASTER and MEM/IO space accesses for NV20 and P2P.
;
; Note, because of a limitation in COMMAND_READ_MEM, we can't read from an
; address larger than 28bits, so we need to make our register base mapping
; under this (0x0D000000), but there's likely kernel code that assumes we're
; at 0xFD000000, so we'll reload to this address at the end of the inittbl.
;
nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_CPU_MEMTOP, CR_CPU_MEMTOP_LIMIT_128MB>    ; for proper P2P addr decode

nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_APC_P2P_8,  CR_APC_P2P_8_MLIMIT_MBASE_INIT>
nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_APC_P2P_9,  CR_APC_P2P_9_PREF_MLIMIT_MBASE>
nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_NV20_PCI_4, NV20_REG_BASE>    ; set register BAR0
nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_NV20_PCI_5, NV20_FB_BASE>     ; set framebuffer BAR1
nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_NV20_PCI_1, CR_NV20_PCI_1_BUS_MSTR_MEM_IO>
nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_APC_P2P_1,  CR_APC_P2P_1_BUS_MSTR_MEM_IO>  ; enable spaces

;
; Also, setup the PBUS_DEBUG_CTRIM values
;
nv_command_struct<COMMAND_READ_MEM, (NV20_REG_BASE + NV_PMC_BOOT_0), DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT, 0000000FFh, 000000000h>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, 0A1h, (pbus_ctrim_A2-$-4)>    ; (rev >= A2)
;
pbus_ctrim_A1:
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_DEBUG_CTRIM_0),    007633451h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_DEBUG_CTRIM_6),    000000000h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_DEBUG_CTRIM_2),    0FFFF0000h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_DEBUG_CTRIM_7),    000000005h>
nv_command_struct<COMMAND_JMP, DONT_CARE, (pbus_ctrim_common-$-4)>

pbus_ctrim_A2:
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_DEBUG_CTRIM_0),    007633461h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_DEBUG_CTRIM_6),    066660000h>

IFDEF SYS_DVT6
;
; PBUS_DEBUG_CTRIM_2 is different between Samsung/Micron
;
nv_command_struct<COMMAND_READ_MEM, (NV20_REG_BASE + NV_PEXTDEV_BOOT_0), DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT, 0000C0000h, 000000000h>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, 0h, (samsung_ctrim2-$-4)> 

micron_ctrim2:
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_DEBUG_CTRIM_2),    0EEEE0000h>
nv_command_struct<COMMAND_JMP, DONT_CARE, (ctrim_continue-$-4)>

samsung_ctrim2:
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_DEBUG_CTRIM_2),    0FFFF0000h>

ctrim_continue:
ELSE
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_DEBUG_CTRIM_2),    000000000h>
ENDIF
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_DEBUG_CTRIM_7),    000000009h>

pbus_ctrim_common:
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_DEBUG_CTRIM_1),    000000000h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_DEBUG_CTRIM_3),    000005866h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_DEBUG_CTRIM_4),    00351C858h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_DEBUG_CTRIM_5),    030007D67h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_DEBUG_CTRIM_8),    000000000h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_DEBUG_CTRIM_9),    0A0423635h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_DEBUG_CTRIM_10),   00C6558C6h>

;
; Before we do MRS_EXT/EMRS_EXT cycles, make sure we've enabled the external bank
;
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_CFG0),              003070103h>

;
; Setup WBC High water mark 
; The NV_PFB_WBC_HWM field should always be set no greater than 0x17
;
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_WBC),               011000016h>

;
; FB Arbitration settings 
;
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_ARB_XFER_SZ),       084848888h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_ARB_TIMEOUT),       0FFFFCFFFh>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_ARB_PREDIVIDER),    01h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_ARB_DIFF_BANK),     0DFh>


;
; Update PEXTDEV_BOOT_0 (if A01)
;
;    0x80354C01 = turn on the override, disables AGP SBA/FW
;    0x803D4C01 = also sets EMRS_MATCHED
;    0x803D4C01 = no fast writes
;
nv_command_struct<COMMAND_READ_MEM, (NV20_REG_BASE + NV_PMC_BOOT_0), DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT, 0000000FFh, 000000000h>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, 0A1h, (skip_strap_override-$-4)>    ; (rev >= A2)

nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PEXTDEV_BOOT_0),        0803D4401h>
skip_strap_override:

;
; Setup the SMBus for talking to the encoder.
;
; On the DVT boards controller 0 isn't used, so only setup controller 1. Besides
; enabling IO's, we'll also convert the pins from GPIOs to SMBus clk/data. This
; need to be done early during boot, so there's a pixclk before setting NVCLK.
;
nv_command_struct <COMMAND_PCI_CFG_WRITE, MCP_SMB_CFG_1,     MCP_SMB_CFG_1_IO_SPACE>
nv_command_struct <COMMAND_PCI_CFG_WRITE, MCP_SMB_CFG_5,     MCP_SMB_CFG_5_IO_BASE>
nv_command_struct <COMMAND_PCI_CFG_WRITE, MCP_SMB_CFG_6,     MCP_SMB_CFG_6_IO_BASE>
nv_command_struct <COMMAND_OUT_BYTE,      SMB_DDC_SPIO_BASE, 70h>    ; converts from GPIO

IFNDEF FOCUS                                                                          
;
; Now, make sure the TV encoder is driving a pixclk.
;
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+4, 08Ah>    ; CX871 slave addr

; CX871 offset 0xBA = 0x3F
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+8, 0BAh>
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+6, 03Fh>    ; turn on slave bit
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+2, 00Ah>    ; kickoff

nv_command_struct <COMMAND_IN_BYTE,  SMB_CONTROL_BASE, DONT_CARE> ; read back status
nv_command_struct <COMMAND_COMPARE_RESULT_JNE, 010h,     -18>     ; spin if not idle
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE, 010h>      ; clear status

; CX871 offset 0x6C = 0x46
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+8, 06Ch>
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+6, 046h>
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+2, 00Ah>    ; write byte kickoff

nv_command_struct <COMMAND_IN_BYTE,  SMB_CONTROL_BASE, DONT_CARE> ; read back status
nv_command_struct <COMMAND_COMPARE_RESULT_JNE, 010h,     -18>     ; spin if not idle
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE, 010h>      ; clear status

; CX871 offset 0xB8 = 0x0
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+8, 0B8h>
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+6, 000h>    ; autoconfig
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+2, 00Ah>    ; kickoff

nv_command_struct <COMMAND_IN_BYTE,  SMB_CONTROL_BASE, DONT_CARE> ; read back status
nv_command_struct <COMMAND_COMPARE_RESULT_JNE, 010h,     -18>     ; spin if not idle
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE, 010h>      ; clear status

; CX871 offset 0xCE = 0x19
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+8, 0CEh>
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+6, 019h>
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+2, 00Ah>    ; kickoff

nv_command_struct <COMMAND_IN_BYTE,  SMB_CONTROL_BASE, DONT_CARE> ; read back status
nv_command_struct <COMMAND_COMPARE_RESULT_JNE, 010h,     -18>     ; spin if not idle
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE, 010h>      ; clear status

; CX871 offset 0xC6 = 0x9C
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+8, 0C6h>
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+6, 09Ch>
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+2, 00Ah>    ; kickoff

nv_command_struct <COMMAND_IN_BYTE,  SMB_CONTROL_BASE, DONT_CARE> ; read back status
nv_command_struct <COMMAND_COMPARE_RESULT_JNE, 010h,     -18>     ; spin if not idle
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE, 010h>      ; clear status

; CX871 offset 0x32 = 0x08
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+8, 032h>
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+6, 008h>
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+2, 00Ah>    ; kickoff

nv_command_struct <COMMAND_IN_BYTE,  SMB_CONTROL_BASE, DONT_CARE> ; read back status
nv_command_struct <COMMAND_COMPARE_RESULT_JNE, 010h,     -18>     ; spin if not idle
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE, 010h>      ; clear status

; CX871 offset 0xC4 = 0x01
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+8, 0C4h>
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+6, 001h>    ; enable clk output
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+2, 00Ah>    ; kickoff

nv_command_struct <COMMAND_IN_BYTE,  SMB_CONTROL_BASE, DONT_CARE> ; read back status
nv_command_struct <COMMAND_COMPARE_RESULT_JNE, 010h,     -18>     ; spin if not idle
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE, 010h>      ; clear status

ENDIF


IFDEF SYS_RETAIL
;
; Verify SMC revision
;
nv_command_struct<COMMAND_OUT_BYTE, SMB_CONTROL_BASE+4, 020h>     ; SMC slave write addr
nv_command_struct<COMMAND_OUT_BYTE, SMB_CONTROL_BASE+8, 001h>     ; SMC revision register
nv_command_struct<COMMAND_OUT_BYTE, SMB_CONTROL_BASE+6, 000h>     ; Clear SMC rev pointer
nv_command_struct<COMMAND_OUT_BYTE, SMB_CONTROL_BASE+2, 00Ah>     ; kickoff
nv_command_struct<COMMAND_IN_BYTE,  SMB_CONTROL_BASE, DONT_CARE>  ; read back status
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, 010h,     -18>      ; spin if not idle
nv_command_struct<COMMAND_OUT_BYTE, SMB_CONTROL_BASE, 010h>       ; clear status

nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+4, 021h>    ; SMC slave read addr
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+8, 001h>    ; read revision register
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE+2, 00Ah>    ; kickoff
nv_command_struct <COMMAND_IN_BYTE,  SMB_CONTROL_BASE, DONT_CARE> ; read back status
nv_command_struct <COMMAND_COMPARE_RESULT_JNE, 010h,     -18>     ; spin if not idle
nv_command_struct <COMMAND_OUT_BYTE, SMB_CONTROL_BASE, 010h>      ; clear status
nv_command_struct <COMMAND_IN_BYTE, SMB_CONTROL_BASE+6, DONT_CARE>   ; get the result

nv_command_struct <COMMAND_COMPARE_RESULT_JNE, DESIRED_SMC_VER, (SmcIsWrong-$-4)> ; if smc is bad
nv_command_struct<COMMAND_JMP, DONT_CARE, (SmcIsOK-$-4)>          ; smc is fine

SmcIsWrong:
;
; SMC is bad, flatline the clocks
;
nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_CPU_MPLL_COEFF, 001000000h>  

SmcIsOK:

ENDIF

;
; Set NVCLK (M, N, P may need adjusting if integrated NV20 VCOs are different)
;    0x0001ED08 = 200MHz
;    0x00011701 = 155MHz    (rev == 0xA01)
;    0x00011C01 = 189MHz
;    0x00011E01 = 202.5MHz   (rev >= 0xA02)
;    0x00011F01 = 209.25MHz  (rev >= 0xA02)
;    0x00014502 = 229.5MHz   (rev >= 0xA02 and 1.7V w/ fansink DVT3)
;
;nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PRAMDAC_NVPLL_COEFF),      00001ED08h>
;
nv_command_struct<COMMAND_READ_MEM, (NV20_REG_BASE + NV_PMC_BOOT_0), DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT, 0000000FFh, 000000000h>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, 0A1h, (nvclk_200-$-4)>    ; (rev >= A2)

nvclk_155:
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PRAMDAC_NVPLL_COEFF),      000011701h>
nv_command_struct<COMMAND_JMP, DONT_CARE, (pll_select-$-4)>

nvclk_200:

nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PRAMDAC_NVPLL_COEFF),      000011C01h>

pll_select:
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PRAMDAC_PLL_COEFF_SELECT), 0000A0400h>

;
; Disable FBIO/TVD calibration
;
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_FBIO_CALEN),  NV_PBUS_FBIO_CALEN_OFF>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_FBIO_CALSEL), NV_PBUS_FBIO_CALSEL_VALUE>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_TVDIO_CALEN), NV_PBUS_TVDIO_CALEN_OFF>

;
; Set the ADR_EDGE_HALF bit in FBIO_CFG
;
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_FBIO_CFG), 000000010h>

;
; DRAM configuration
;
; Determine Samsung vs Micron by checking NV_PEXTDEV_BOOT_0_STRAP_EMRS (19:18)
; 0=Micron, 3=Samsung
;
nv_command_struct<COMMAND_READ_MEM, (NV20_REG_BASE + NV_PEXTDEV_BOOT_0), DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT, 0000C0000h, 000000000h>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, 0h, (samsung_memory-$-4)> 

;
; Configure for Micron
;
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_FBIO_DLY),    028282828h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_FBIO_ADRDRV), 088888888h>
nv_command_struct<COMMAND_JMP, DONT_CARE, (memory_pad_config-$-4)>

samsung_memory:
;
; Configure for Samsung
;
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_FBIO_DLY),    009090909h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_FBIO_ADRDRV), 0AAAAAAAAh> 

memory_pad_config:

;
; Memory PAD configuration
;
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_FBIO_CLKDRV), 0FFFFFFFFh>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_FBIO_DATDRV), NV_PBUS_FBIO_DATDRV_INIT>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_FBIO_DQSDRV), NV_PBUS_FBIO_DQSDRV_INIT>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_FBIO_ADRSLW), NV_PBUS_FBIO_ADRSLW_INIT>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_FBIO_CLKSLW), 0FFFFFFFFh>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_FBIO_DATSLW), NV_PBUS_FBIO_DATSLW_INIT>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_FBIO_DQSSLW), NV_PBUS_FBIO_DQSSLW_INIT>

;
; RAM Initialization (XXX should we be waiting for cmds to complete?)
;
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_PRE),      NV_PFB_PRE_CMD_PRECHARGE_1>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_EMRS),     000100042h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_EMRS_EXT), 000100042h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_MRS),      000000011h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_MRS_EXT),  000000011h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_MRS),      000000032h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_MRS_EXT),  000000032h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_MRS),      NV_PFB_MRS_DLL_RESET>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_MRS_EXT),  NV_PFB_MRS_EXT_DLL_RESET>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_REF),      000000001h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_REF),      000000001h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_REFCTRL),  NV_PFB_REFCTRL_VALID_1>

;
; Display PAD configuration
;
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_DISPIO_PADCTL), NV_PBUS_DISPIO_PADCTL_INIT>

;
; TVD PAD configuration
;
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_TVDIO_PADCTL), NV_PBUS_TVDIO_PADCTL_INIT>

;
; Change PFB_TIMING2 and PBUS_FBIO_RAM
;
; NV_PFB_TIMING2 = 0x081202FF
;                = 0x081205FF    (increase REFRESH count with M=1, N=9, MEMPDIV=2)
;
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_TIMING2),   0081205FFh>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PBUS_FBIO_RAM), 000010000h>

;
; Tell MCP not to generate INIT# in response to the shutdown cycle 
;
nv_command_struct<COMMAND_PCI_CFG_READ, MCP_LEG_CFG_24, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT, 0FFFFFFFFh, 000000400h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, MCP_LEG_CFG_24>

;
; Init ISO/non-ISO bits for the various MCP unit IDs
;
nv_command_struct <COMMAND_PCI_CFG_WRITE, MCP_LEG_CFG_19, 00000FDDEh>

;
; Make sure the USB PLL has the correct value (not the POR value).
;
; nv_command_struct <COMMAND_PCI_CFG_WRITE, MCP_LEG_CFG_39, 0851CAA0Ch>
nv_command_struct <COMMAND_PCI_CFG_WRITE, MCP_LEG_CFG_39, 0871CC707h>

;
; Set the maximum PCICLK_OUT5 trim (bits 11:8 = 0xF).
;
nv_command_struct<COMMAND_PCI_CFG_READ, MCP_LEG_CFG_45, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT,   0FFFFF0FFh, 000000F00h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, MCP_LEG_CFG_45>

;
; Set cshape/ctrim for clocks in CR_top.
;
nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_CPU_CST_0, 0F0F0C0C0h>    ; A03
nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_CPU_CST_1, 000C00000h>
nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_CPU_CST_7, 004070000h>

;
; These values have been set by the straps, but we'll write them into the
; CR_CPU_MPLL_COEFF register, so the values are knowable outside the ROM.
;
; Currently, we have MEM_PDIV of 3, which is 133MHz and will be left that
; way for A01. For A02, we set the MEM_PDIV to 2, which sets a 200MHz mclk.
;
;    (0x230901: MEM_PDIV = 2, FSB_PDIV = 3; NDIV = 9;  MDIV = 1)
;    (0x230A01: MEM_PDIV = 2, FSB_PDIV = 3; NDIV = A;  MDIV = 1)
;    (0x231D03: MEM_PDIV = 2, FSB_PDIV = 3; NDIV = 1D; MDIV = 3)
;
; nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_CPU_MPLL_COEFF, 000230901h>
; nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_CPU_MPLL_COEFF, 000230901h>    ; 182MHz
; nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_CPU_MPLL_COEFF, 000231D03h>    ; 200MHz
; nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_CPU_MPLL_COEFF, 000230A01h>    ; 202MHz
;
nv_command_struct<COMMAND_READ_MEM, (NV20_REG_BASE + NV_PMC_BOOT_0), DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT, 0000000FFh, 000000000h>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, 0A1h, (mclk_200-$-4)>    ; (rev >= A2)
nv_command_struct<COMMAND_JMP, DONT_CARE, (mclk_133-$-4)>              ; (rev == A1)

mclk_133:
mclk_200:
;
; Set the clocks and override. CR_CPU_MPLL_COEFF must be set once without override
; and then again with override. 
;
nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_CPU_MPLL_COEFF, 000230801h>
nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_CPU_MPLL_COEFF, 001230801h>  

;
; Delay ~15 nano seconds by performing jmps
; 
nv_command_struct< COMMAND_JMP, DONT_CARE, (DelayJump1-$-4) >
DelayJump1:
nv_command_struct< COMMAND_JMP, DONT_CARE, (DelayJump2-$-4) >
DelayJump2:

; MEMORY TEST
; 
; This section does memory detection and test, and then updates
; the memory config registers and reports this config (and memory errors) back
; to the SMC.
;
; First, this is how the addressing works, with 12 row bits and an
; external bank:
;
;    E[0:0],R[11:10] | R[9:6] | R[5:2] | R[1:0],B[1:0] | C[7:4] | C[3:2],P[1:0] | C[1:0],00
;
; We determine a particular DRAMs number of row bits (11 or 12) by toggling
; the R11 bit and looking for an alias within the DRAM (row address bit doesn't
; exist with the 2Mx32 parts).
;
; For row detection with the first 4 DRAM partitions (MEM_PART_0 -> MEM_PART_3),
; we'll address at 0x2500000 (R11 = 1, ext_bank = 0) and cycle the partition bits.
; For the second set of DRAMS (MEM_PART_4 -> MEM_PART_7), we'll address at
; 0x6500000 (R11 = 1, ext_bank = 1).
;
; In addition to row detection, we'll update the SMC controller with the number
; of 2Mx32 and 4Mx32 DRAMS and any memory errors we've found. We use the 2 USB
; registers as scratchpad (reset to 0 and later overwritten by the kernel).
;
;    rows = 2^12, cols = 2^8, banks = 2^2, external bank
;


nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_CFG0),  003070103h>
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_CFG1),  011448000h>

;
; Clear the scratchpad registers used in memory test
;
nv_command_struct<COMMAND_PCI_CFG_WRITE, MEMTEST_TYPE, 0>
nv_command_struct<COMMAND_PCI_CFG_WRITE, MEMTEST_RESULT, 0>

;-------------------------------------------------------------------------------
dram0:
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_0 + 000555548h), MEMTEST_PATTERN1>
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_0 + 002555548h), MEMTEST_PATTERN2>

; does the DRAM exist?
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_0 + 002555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN2, (dram0_err-$-4)>

; did we alias in the DRAM?
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_0 + 000555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN1, (dram0_alt_write-$-4)>

dram0_4mb:
nv_command_struct<COMMAND_PCI_CFG_READ, MEMTEST_TYPE, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT,   0FFFFFFFFh, 000000001h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, MEMTEST_TYPE>

dram0_alt_write:
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_0 + 000555548h), MEMTEST_PATTERN3>
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_0 + 000555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN3, (dram0_err-$-4)>
nv_command_struct<COMMAND_JMP, DONT_CARE, (dram1-$-4)>

dram0_err:
nv_command_struct<COMMAND_PCI_CFG_READ, MEMTEST_RESULT, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT,   0FFFFFFFFh, 000000001h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, MEMTEST_RESULT>


;-------------------------------------------------------------------------------
dram1:
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_1 + 000555548h), MEMTEST_PATTERN1>
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_1 + 002555548h), MEMTEST_PATTERN2>

; does the DRAM exist?
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_1 + 002555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN2, (dram1_err-$-4)>

; did we alias in the DRAM?
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_1 + 000555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN1, (dram1_alt_write-$-4)>

dram1_4mb:
nv_command_struct<COMMAND_PCI_CFG_READ, MEMTEST_TYPE, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT,   0FFFFFFFFh, 000000002h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, MEMTEST_TYPE>

dram1_alt_write:
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_1 + 000555548h), MEMTEST_PATTERN3>
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_1 + 000555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN3, (dram1_err-$-4)>
nv_command_struct<COMMAND_JMP, DONT_CARE, (dram2-$-4)>

dram1_err:
nv_command_struct<COMMAND_PCI_CFG_READ, MEMTEST_RESULT, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT,   0FFFFFFFFh, 000000002h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, MEMTEST_RESULT>


;-------------------------------------------------------------------------------
dram2:
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_2 + 000555548h), MEMTEST_PATTERN1>
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_2 + 002555548h), MEMTEST_PATTERN2>

; does the DRAM exist?
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_2 + 002555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN2, (dram2_err-$-4)>

; did we alias in the DRAM?
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_2 + 000555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN1, (dram2_alt_write-$-4)>

dram2_4mb:
nv_command_struct<COMMAND_PCI_CFG_READ, MEMTEST_TYPE, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT,   0FFFFFFFFh, 000000004h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, MEMTEST_TYPE>

dram2_alt_write:
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_2 + 000555548h), MEMTEST_PATTERN3>
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_2 + 000555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN3, (dram2_err-$-4)>
nv_command_struct<COMMAND_JMP, DONT_CARE, (dram3-$-4)>

dram2_err:
nv_command_struct<COMMAND_PCI_CFG_READ, MEMTEST_RESULT, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT,   0FFFFFFFFh, 000000004h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, MEMTEST_RESULT>


;-------------------------------------------------------------------------------
dram3:
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_3 + 000555548h), MEMTEST_PATTERN1>
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_3 + 002555548h), MEMTEST_PATTERN2>

; does the DRAM exist?
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_3 + 002555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN2, (dram3_err-$-4)>

; did we alias in the DRAM?
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_3 + 000555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN1, (dram3_alt_write-$-4)>

dram3_4mb:
nv_command_struct<COMMAND_PCI_CFG_READ, MEMTEST_TYPE, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT,   0FFFFFFFFh, 000000008h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, MEMTEST_TYPE>

dram3_alt_write:
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_3 + 000555548h), MEMTEST_PATTERN3>
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_3 + 000555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN3, (dram3_err-$-4)>
nv_command_struct<COMMAND_JMP, DONT_CARE, (dram4-$-4)>

dram3_err:
nv_command_struct<COMMAND_PCI_CFG_READ, MEMTEST_RESULT, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT,   0FFFFFFFFh, 000000008h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, MEMTEST_RESULT>


;-------------------------------------------------------------------------------
dram4:
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_4 + 000555548h), MEMTEST_PATTERN1>
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_4 + 002555548h), MEMTEST_PATTERN2>

; does the DRAM exist?
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_4 + 002555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN2, (dram4_err-$-4)>

; did we alias in the DRAM?
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_4 + 000555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN1, (dram4_alt_write-$-4)>

dram4_4mb:
nv_command_struct<COMMAND_PCI_CFG_READ, MEMTEST_TYPE, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT,   0FFFFFFFFh, 000000010h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, MEMTEST_TYPE>

dram4_alt_write:
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_4 + 000555548h), MEMTEST_PATTERN3>
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_4 + 000555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN3, (dram4_err-$-4)>
nv_command_struct<COMMAND_JMP, DONT_CARE, (dram5-$-4)>

dram4_err:
nv_command_struct<COMMAND_PCI_CFG_READ, MEMTEST_RESULT, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT,   0FFFFFFFFh, 000000010h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, MEMTEST_RESULT>


;-------------------------------------------------------------------------------
dram5:
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_5 + 000555548h), MEMTEST_PATTERN1>
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_5 + 002555548h), MEMTEST_PATTERN2>

; does the DRAM exist?
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_5 + 002555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN2, (dram5_err-$-4)>

; did we alias in the DRAM?
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_5 + 000555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN1, (dram5_alt_write-$-4)>

dram5_4mb:
nv_command_struct<COMMAND_PCI_CFG_READ, MEMTEST_TYPE, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT,   0FFFFFFFFh, 000000020h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, MEMTEST_TYPE>

dram5_alt_write:
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_5 + 000555548h), MEMTEST_PATTERN3>
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_5 + 000555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN3, (dram5_err-$-4)>
nv_command_struct<COMMAND_JMP, DONT_CARE, (dram6-$-4)>

dram5_err:
nv_command_struct<COMMAND_PCI_CFG_READ, MEMTEST_RESULT, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT,   0FFFFFFFFh, 000000020h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, MEMTEST_RESULT>


;-------------------------------------------------------------------------------
dram6:
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_6 + 000555548h), MEMTEST_PATTERN1>
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_6 + 002555548h), MEMTEST_PATTERN2>

; does the DRAM exist?
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_6 + 002555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN2, (dram6_err-$-4)>

; did we alias in the DRAM?
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_6 + 000555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN1, (dram6_alt_write-$-4)>

dram6_4mb:
nv_command_struct<COMMAND_PCI_CFG_READ, MEMTEST_TYPE, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT,   0FFFFFFFFh, 000000040h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, MEMTEST_TYPE>

dram6_alt_write:
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_6 + 000555548h), MEMTEST_PATTERN3>
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_6 + 000555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN3, (dram6_err-$-4)>
nv_command_struct<COMMAND_JMP, DONT_CARE, (dram7-$-4)>

dram6_err:
nv_command_struct<COMMAND_PCI_CFG_READ, MEMTEST_RESULT, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT,   0FFFFFFFFh, 000000040h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, MEMTEST_RESULT>


;-------------------------------------------------------------------------------
dram7:
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_7 + 000555548h), MEMTEST_PATTERN1>
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_7 + 002555548h), MEMTEST_PATTERN2>

; does the DRAM exist?
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_7 + 002555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN2, (dram7_err-$-4)>

; did we alias in the DRAM?
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_7 + 000555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN1, (dram7_alt_write-$-4)>

dram7_4mb:
nv_command_struct<COMMAND_PCI_CFG_READ, MEMTEST_TYPE, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT,   0FFFFFFFFh, 000000080h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, MEMTEST_TYPE>

dram7_alt_write:
nv_command_struct<COMMAND_WRITE_MEM, (MEM_PART_7 + 000555548h), MEMTEST_PATTERN3>
nv_command_struct<COMMAND_READ_MEM,  (MEM_PART_7 + 000555548h), DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, MEMTEST_PATTERN3, (dram7_err-$-4)>
nv_command_struct<COMMAND_JMP, DONT_CARE, (memtest_done-$-4)>

dram7_err:
nv_command_struct<COMMAND_PCI_CFG_READ, MEMTEST_RESULT, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT,   0FFFFFFFFh, 000000080h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_PCI_CFG_WRITE, MEMTEST_RESULT>


memtest_done:
;
; Start by assuming we're using a 4Mx32, 64MB config (no external bank).
; We've already set rows = 2^12, so clear the external bank bit.
;
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_CFG0),  003070003h>
nv_command_struct<COMMAND_PCI_CFG_WRITE, CR_CPU_MEMTOP, CR_CPU_MEMTOP_LIMIT_64MB>

;
; Read out our bitmask of 4Mx32 parts into the SMBus register and update our
; memory config, if necessary.
; 
nv_command_struct<COMMAND_PCI_CFG_READ, MEMTEST_TYPE, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT,   0000000FFh, 000000000h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_OUT_BYTE, SMB_CONTROL_BASE+6>

nv_command_struct<COMMAND_IN_BYTE,  SMB_CONTROL_BASE+6, DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, 000000000h, (dram_4M-$-4)>

; Reset back to 11 row bits and add back the external bank bit
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_CFG0),  003070103h>    ; extbank bit
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_CFG1),  011338000h>    ; 11 row bits
nv_command_struct<COMMAND_JMP, DONT_CARE, (mem_report-$-4)>

dram_4M:
; We've got some 4Mx32 parts, check if we're a 128MB config?
nv_command_struct<COMMAND_IN_BYTE,  SMB_CONTROL_BASE+6, DONT_CARE>
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, 0000000FFh, (mem_report-$-4)>

; Add back the external bank bit and update MEMTOP
nv_command_struct<COMMAND_WRITE_MEM, (NV20_REG_BASE + NV_PFB_CFG0),  003070103h>    ; extback bit
nv_command_struct<COMMAND_PCI_CFG_WRITE, CR_CPU_MEMTOP, CR_CPU_MEMTOP_LIMIT_128MB>


mem_report:

;
; Report the results to the SMC
;
nv_command_struct<COMMAND_OUT_BYTE, SMB_CONTROL_BASE+4, 020h>     ; SMC slave addr

;
; Send Memory Type (already in SMB_CONTROL_BASE+6) to SMC
;
nv_command_struct<COMMAND_OUT_BYTE, SMB_CONTROL_BASE+8, 013h>     ; SMC Register
nv_command_struct<COMMAND_OUT_BYTE, SMB_CONTROL_BASE+2, 00Ah>     ; kickoff

nv_command_struct<COMMAND_IN_BYTE,  SMB_CONTROL_BASE, DONT_CARE>  ; read back status
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, 010h,     -18>      ; spin if not idle
nv_command_struct<COMMAND_OUT_BYTE, SMB_CONTROL_BASE, 010h>       ; clear status

;
; Send Memory test results to SMC
;
nv_command_struct<COMMAND_PCI_CFG_READ, MEMTEST_RESULT, DONT_CARE>
nv_command_struct<COMMAND_RMW_RESULT,   0000000FFh, 000000000h>
nv_command_struct<COMMAND_USE_RESULT_AS_DATA, COMMAND_OUT_BYTE, SMB_CONTROL_BASE+6>

nv_command_struct<COMMAND_OUT_BYTE, SMB_CONTROL_BASE+8, 012h>     ; SMC Register
nv_command_struct<COMMAND_OUT_BYTE, SMB_CONTROL_BASE+2, 00Ah>     ; kickoff

nv_command_struct<COMMAND_IN_BYTE,  SMB_CONTROL_BASE, DONT_CARE>  ; read back status
nv_command_struct<COMMAND_COMPARE_RESULT_JNE, 010h,     -18>      ; spin if not idle
nv_command_struct<COMMAND_OUT_BYTE, SMB_CONTROL_BASE, 010h>       ; clear status

mem_report_end:


;
; Reload the NV20 register base (should be just prior to COMMAND_QUIT)
; to what the kernel expects.
;
nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_APC_P2P_8,  CR_APC_P2P_8_MLIMIT_MBASE_KERNEL>
nv_command_struct <COMMAND_PCI_CFG_WRITE, CR_NV20_PCI_4, NV20_REG_BASE_KERNEL>

; End of table.  First argument is used as the value for the default MTRR register
;
nv_command_struct <COMMAND_QUIT, 0806h, DONT_CARE>

;
; Drive/Slew Parameter Offset and Data.  Boot loader needs to access this data
; 

org DATATBL_BASE_OFFSET  

;DATATBL_DRVSLWPARAM_OFFSET   EQU DATATBL_BASE_OFFSET+0

;-----------------------------------------------------------------------------

DrvSlwParamBegin:
;
; Common calibration data
;
dw    20000    ; MaxMClk (230Mhz)
;
; Operating condition based on speedometer
;     (Count is from VCCQ; CountB is from VCC)
;

;
; 16.6 clock
;

db    4Eh      ; SlowCountExt
db    3Eh      ; SlowCountBExt

db    56h      ; SlowCountAvg
db    43h      ; SlowCountBAvg

db    5Dh      ; TypiCount
db    4Ah      ; TypiCountB

db    64h      ; FastCountAvg
db    50h      ; FastCountBAvg

db    6Ah      ; FastCountExt
db    55h      ; FastCountBExt

;-----------------------------------------------------------------------------
;
; Samsung Extreme FAST values 
;
db    09h      ; AdrDrvFallFastExt
db    09h      ; AdrDrvRiseFastExt
db    0Eh      ; AdrSlwFallFastExt
db    08h      ; AdrSlwRiseFastExt

db    0Fh      ; ClkDrvFallFastExt
db    0Fh      ; ClkDrvRiseFastExt
db    0Fh      ; ClkSlwFallFastExt
db    0Fh      ; ClkSlwRiseFastExt

db    09h      ; DatDrvFallFastExt
db    09h      ; DatDrvRiseFastExt
db    0Eh      ; DatSlwFallFastExt
db    08h      ; DatSlwRiseFastExt

db    09h      ; DqsDrvFallFastExt
db    09h      ; DqsDrvRiseFastExt
db    0Eh      ; DqsSlwFallFastExt
db    08h      ; DqsSlwRiseFastExt

IFDEF SYS_DVT6
db    01h      ; DataInbDelyFastExt
db    00h      ; ClkIcDelyFastExt
db    00h      ; DqsInbDelyFastExt
ELSE
db    00h      ; DataInbDelyFastExt
db    00h      ; ClkIcDelyFastExt
db    01h      ; DqsInbDelyFastExt
ENDIF
;
; Samsung Average FAST values
;
db    09h      ; AdrDrvFallFastAvg
db    09h      ; AdrDrvRiseFastAvg
db    0Eh      ; AdrSlwFallFastAvg
db    08h      ; AdrSlwRiseFastAvg

db    0Fh      ; ClkDrvFallFastAvg
db    0Fh      ; ClkDrvRiseFastAvg
db    0Fh      ; ClkSlwFallFastAvg
db    0Fh      ; ClkSlwRiseFastAvg

db    09h      ; DatDrvFallFastAvg
db    09h      ; DatDrvRiseFastAvg
db    0Eh      ; DatSlwFallFastAvg
db    08h      ; DatSlwRiseFastAvg

db    09h      ; DqsDrvFallFastAvg
db    09h      ; DqsDrvRiseFastAvg
db    0Eh      ; DqsSlwFallFastAvg
db    08h      ; DqsSlwRiseFastAvg

IFDEF SYS_DVT6
db    01h      ; DataInbDelyFastAvg
db    01h      ; ClkIcDelyFastAvg
db    00h      ; DqsInbDelyFastAvg 
ELSE
db    00h      ; DataInbDelyFastAvg
db    01h      ; ClkIcDelyFastAvg
db    01h      ; DqsInbDelyFastAvg 
ENDIF
;
; Samsung Typical values
;
db    09h      ; AdrDrvFallTypi
db    09h      ; AdrDrvRiseTypi
db    0Eh      ; AdrSlwFallTypi
db    08h      ; AdrSlwRiseTypi

db    0Fh      ; ClkDrvFallTypi
db    0Fh      ; ClkDrvRiseTypi
db    0Fh      ; ClkSlwFallTypi
db    0Fh      ; ClkSlwRiseTypi

db    09h      ; DatDrvFallTypi
db    09h      ; DatDrvRiseTypi
db    0Eh      ; DatSlwFallTypi
db    08h      ; DatSlwRiseTypi

db    09h      ; DqsDrvFallTypi
db    09h      ; DqsDrvRiseTypi
db    0Eh      ; DqsSlwFallTypi
db    08h      ; DqsSlwRiseTypi

IFDEF SYS_DVT6
db    01h      ; DataInbDelyTypi
db    02h      ; ClkIcDelyTypi
db    00h      ; DqsInbDelyTypi
ELSE
db    00h      ; DataInbDelyTypi
db    02h      ; ClkIcDelyTypi
db    01h      ; DqsInbDelyTypi
ENDIF
;
; Samsung Average SLOW values
;
db    0Ah      ; AdrDrvFallSlowAvg
db    0Bh      ; AdrDrvRiseSlowAvg
db    0Eh      ; AdrSlwFallSlowAvg
db    08h      ; AdrSlwRiseSlowAvg

db    0Fh      ; ClkDrvFallSlowAvg
db    0Fh      ; ClkDrvRiseSlowAvg
db    0Fh      ; ClkSlwFallSlowAvg
db    0Fh      ; ClkSlwRiseSlowAvg

db    0Ah      ; DatDrvFallSlowAvg
db    0Bh      ; DatDrvRiseSlowAvg
db    0Eh      ; DatSlwFallSlowAvg
db    08h      ; DatSlwRiseSlowAvg

db    0Ah      ; DqsDrvFallSlowAvg
db    0Bh      ; DqsDrvRiseSlowAvg
db    0Eh      ; DqsSlwFallSlowAvg
db    08h      ; DqsSlwRiseSlowAvg

IFDEF SYS_DVT6
db    01h      ; DataInbDelySlowAvg
db    02h      ; ClkIcDelySlowAvg
db    00h      ; DqsInbDelySlowAvg
ELSE
db    00h      ; DataInbDelySlowAvg
db    02h      ; ClkIcDelySlowAvg
db    01h      ; DqsInbDelySlowAvg
ENDIF
;
; Samsung Extreme SLOW values
;
db    0Bh      ; AdrDrvFallSlowExt
db    0Eh      ; AdrDrvRiseSlowExt
db    0Eh      ; AdrSlwFallSlowExt
db    08h      ; AdrSlwRiseSlowExt

db    0Fh      ; ClkDrvFallSlowExt
db    0Fh      ; ClkDrvRiseSlowExt
db    0Fh      ; ClkSlwFallSlowExt
db    0Fh      ; ClkSlwRiseSlowExt

db    0Bh      ; DatDrvFallSlowExt
db    0Eh      ; DatDrvRiseSlowExt
db    0Eh      ; DatSlwFallSlowExt
db    08h      ; DatSlwRiseSlowExt

db    0Bh      ; DqsDrvFallSlowExt
db    0Eh      ; DqsDrvRiseSlowExt
db    0Eh      ; DqsSlwFallSlowExt
db    08h      ; DqsSlwRiseSlowExt

IFDEF SYS_DVT6
db    01h      ; DataInbDelySlowExt
db    03h      ; ClkIcDelySlowExt
db    00h      ; DqsInbDelySlowExt
ELSE
db    00h      ; DataInbDelySlowExt
db    03h      ; ClkIcDelySlowExt
db    01h      ; DqsInbDelySlowExt
ENDIF
;-----------------------------------------------------------------------------
;
; Micron Extreme FAST values
;
db    08h      ; AdrDrvFallFastExt
db    08h      ; AdrDrvRiseFastExt
db    0Eh      ; AdrSlwFallFastExt
db    08h      ; AdrSlwRiseFastExt

db    0Fh      ; ClkDrvFallFastExt
db    0Fh      ; ClkDrvRiseFastExt
db    0Fh      ; ClkSlwFallFastExt
db    0Fh      ; ClkSlwRiseFastExt

db    09h      ; DatDrvFallFastExt
db    09h      ; DatDrvRiseFastExt
db    0Eh      ; DatSlwFallFastExt
db    08h      ; DatSlwRiseFastExt

db    09h      ; DqsDrvFallFastExt
db    09h      ; DqsDrvRiseFastExt
db    0Eh      ; DqsSlwFallFastExt
db    08h      ; DqsSlwRiseFastExt

db    00h      ; DataInbDelyFastExt
db    01h      ; ClkIcDelyFastExt
db    01h      ; DqsInbDelyFastExt
;
; Micron Average FAST values
;
db    08h      ; AdrDrvFallFastAvg
db    08h      ; AdrDrvRiseFastAvg
db    0Eh      ; AdrSlwFallFastAvg
db    08h      ; AdrSlwRiseFastAvg

db    0Fh      ; ClkDrvFallFastAvg
db    0Fh      ; ClkDrvRiseFastAvg
db    0Fh      ; ClkSlwFallFastAvg
db    0Fh      ; ClkSlwRiseFastAvg

db    09h      ; DatDrvFallFastAvg
db    09h      ; DatDrvRiseFastAvg
db    0Eh      ; DatSlwFallFastAvg
db    08h      ; DatSlwRiseFastAvg

db    09h      ; DqsDrvFallFastAvg
db    09h      ; DqsDrvRiseFastAvg
db    0Eh      ; DqsSlwFallFastAvg
db    08h      ; DqsSlwRiseFastAvg

db    00h      ; DataInbDelyFastAvg
db    01h      ; ClkIcDelyFastAvg
db    01h      ; DqsInbDelyFastAvg 
;
; Micron Typical values
;
db    08h      ; AdrDrvFallTypi
db    08h      ; AdrDrvRiseTypi
db    0Eh      ; AdrSlwFallTypi
db    08h      ; AdrSlwRiseTypi

db    0Fh      ; ClkDrvFallTypi
db    0Fh      ; ClkDrvRiseTypi
db    0Fh      ; ClkSlwFallTypi
db    0Fh      ; ClkSlwRiseTypi

db    09h      ; DatDrvFallTypi
db    09h      ; DatDrvRiseTypi
db    0Eh      ; DatSlwFallTypi
db    08h      ; DatSlwRiseTypi

db    09h      ; DqsDrvFallTypi
db    09h      ; DqsDrvRiseTypi
db    0Eh      ; DqsSlwFallTypi
db    08h      ; DqsSlwRiseTypi

db    00h      ; DataInbDelyTypi
db    01h      ; ClkIcDelyTypi
db    01h      ; DqsInbDelyTypi
;
; Micron Average SLOW values
;
db    08h      ; AdrDrvFallSlowAvg
db    08h      ; AdrDrvRiseSlowAvg
db    0Eh      ; AdrSlwFallSlowAvg
db    08h      ; AdrSlwRiseSlowAvg

db    0Fh      ; ClkDrvFallSlowAvg
db    0Fh      ; ClkDrvRiseSlowAvg
db    0Fh      ; ClkSlwFallSlowAvg
db    0Fh      ; ClkSlwRiseSlowAvg

db    0Ah      ; DatDrvFallSlowAvg
db    0Bh      ; DatDrvRiseSlowAvg
db    0Eh      ; DatSlwFallSlowAvg
db    08h      ; DatSlwRiseSlowAvg

db    0Ah      ; DqsDrvFallSlowAvg
db    0Bh      ; DqsDrvRiseSlowAvg
db    0Eh      ; DqsSlwFallSlowAvg
db    08h      ; DqsSlwRiseSlowAvg

db    00h      ; DataInbDelySlowAvg
db    01h      ; ClkIcDelySlowAvg
db    01h      ; DqsInbDelySlowAvg
;
; Micron Extreme SLOW values
;
db    08h      ; AdrDrvFallSlowExt
db    08h      ; AdrDrvRiseSlowExt
db    0Eh      ; AdrSlwFallSlowExt
db    08h      ; AdrSlwRiseSlowExt

db    0Fh      ; ClkDrvFallSlowExt
db    0Fh      ; ClkDrvRiseSlowExt
db    0Fh      ; ClkSlwFallSlowExt
db    0Fh      ; ClkSlwRiseSlowExt

db    0Bh      ; DatDrvFallSlowExt
db    0Eh      ; DatDrvRiseSlowExt
db    0Eh      ; DatSlwFallSlowExt
db    08h      ; DatSlwRiseSlowExt

db    0Bh      ; DqsDrvFallSlowExt
db    0Eh      ; DqsDrvRiseSlowExt
db    0Eh      ; DqsSlwFallSlowExt
db    08h      ; DqsSlwRiseSlowExt

db    00h      ; DataInbDelySlowExt
db    01h      ; ClkIcDelySlowExt
db    01h      ; DqsInbDelySlowExt

DrvSlwParamEnd:

DB   'Copyright (c) Microsoft Corporation. All rights reserved.'


CODE ENDS

