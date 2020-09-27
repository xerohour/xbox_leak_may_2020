;       (C) Copyright NVIDIA Corporation Inc., 1996,1997. All rights reserved.
;       Portions Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.
;
;       NVIDIA.INC - Include file for MiniVDD definitions
;
;       Written by:     Mark Stephen Krueger
;       Date:           10/1/96
;
;       $Revision: 6 $
;       $Date: 5/05/98 6:36p $
;       $Logfile: /resman.050/win96/vdd/nvidia.inc $
;       $Log: /resman.050/win96/vdd/nvidia.inc $
; 
; 6     5/05/98 6:36p Jhinman
; Removed Weitek simulation equates.
; 
; 5     1/14/98 3:04p Jhinman
; Moved API functions to 1xx.
; 
; 4     1/09/98 2:03p Jhinman
; Added some equates for API definitions.
; 
; 3     11/07/97 6:42p Jhinman
; Add equate for nv_device_id.
; 
; 2     10/27/97 3:02p Jhinman
; Define RM_OK (status).
; 
; 1     5/13/97 10:43a Jeffwe
; 
; 1     5/12/97 6:49p Jeffwe
; 
; 6     1/23/97 11:02a Markk
; Removed tabs.
; 
; 5     1/14/97 3:42p Markk
; Added EXT_NV_PDAC_GEN_CTRL and EXT_NV_PFB_CONFIG_0 to save
; corresponding NV3 registers.
; 
; 4     11/15/96 6:05p Markk
; Put in NVIDIA_COMMON stuff for simulating.
; 
; 3     11/13/96 12:28p Markk
; Updated PerVMData for NV3.
; Fixed PCI_DWORD_* constants.
; Added other PCI commands and error codes.
; 
; 2     10/21/96 7:02p Markk
; Added more VGA register definitions.
; Added flag to save structure to know when values saved.
; 
; 1     10/16/96 9:16a Markk
; Include file for the NVidia Mini-VDD.
;

; OS Version numbers
VERS_MILLENNIUM    EQU     045ah
VERS_WIN98         EQU     040AH

; Definitions of ACPI power states. Should be (but aren't) defined in CONFIGMG.INC.
CM_POWERSTATE_D3   EQU     8   
CM_POWERSTATE_D2   EQU     4
CM_POWERSTATE_D1   EQU     2
CM_POWERSTATE_D0   EQU     1
CM_POWERSTATE_HIBERNATE   EQU     10h

; Resource Manager API functions
NVRM_API_ARCHITECTURE_32 equ 1000+73
NVRM_API_DISPLAY         equ 1000+75
NVRM_API_ACPI_ENABLED    equ 1000+82

NVRM_API_ALLOC_FIFO equ     1000+1
NVRM_API_MAP_FIFO   equ     1000+3
MAP_LINEAR          equ     1
NVRM_API_INIT       equ     1000+100
NVRM_API_ENABLEVGA  equ     1000+101
NVRM_API_ENABLEHIRES equ    1000+102
NVRM_API_DEV_HANDLE equ     1000+103
NVRM_API_DEV_HANDLE_2 equ   1000+104
NVRM_API_POWER_UP   equ     1000+105
NVRM_API_POWER_DOWN equ     1000+106
NVRM_API_EXIT       equ     1000+107
NVRM_API_POWER_D1   equ     1000+108
NVRM_API_POWER_D2   equ     1000+109
NVRM_API_READ_EDID  equ     1000+112
NVRM_API_NUM_HEADS  equ     1000+113
NVRM_API_GET_DDC_CAPS equ   1000+114
NVRM_API_HIBERNATE  equ     1000+115
NVRM_API_SET_MONITOR_POWER equ 1000+116
NVRM_API_BLANK_SECONDARY   equ 1000+117   
NVRM_API_DISPLAY_GET_BOOT_PHYS_MAP      equ     1000+118
; RM display types
DISPLAY_TYPE_MONITOR    equ 0
DISPLAY_TYPE_TV         equ 1
DISPLAY_TYPE_FLAT_PANEL equ 3

;
;Chip ID equates:
;
NV31A           equ     DRF_DEF(_PMC,_BOOT_0,_IMPLEMENTATION,_NV3) OR \
                        DRF_DEF(_PMC,_BOOT_0,_MASK_REVISION_A) OR \
                        DRF_DEF(_PMC,_BOOT_0,_FIB_REVISION_0)
  
RM_OK                   equ             0

NV3_DEVICE_ID   EQU             0018H

;
; I/O addresses
;
CRTC_INDEX_MONO         equ     03B4h
CRTC_DATA_MONO          equ     03B5h
CRTC_INDEX_COLOR        equ     03D4h
CRTC_DATA_COLOR         equ     03D5h
INPUT_STATUS_MONO       equ     CRTC_INDEX_MONO + 6
INPUT_STATUS_COLOR      equ     CRTC_INDEX_COLOR + 6
OUTPUT_FC_MONO          equ     CRTC_INDEX_MONO + 6
OUTPUT_FC_COLOR         equ     CRTC_INDEX_COLOR + 6
ATC_INDEX               equ     03C0h
ATC_RDATA               equ     03C1h
MISC_OUTPUT             equ     03C2h
FEAT_STATUS             equ     03C2h
SUBSYS_ENABLE           equ     03C3h
SEQ_INDEX               equ     03C4h
SEQ_DATA                equ     03C5h
DAC_MASK                equ     03C6h
DAC_RINDEX              equ     03C7h
DAC_WINDEX              equ     03C8h
DAC_DATA                equ     03C9h
FC_INPUT                equ     03CAh
MISC_INPUT              equ     03CCh
GDC_INDEX               equ     03CEh
GDC_DATA                equ     03CFh
;
BIOS_SCRATCH1       equ     2Ch         ; BIOS SCRATCH REG
BIOS_PGRM_TV        equ     2           ; BIOS will program TV encoder when CLEAR
NVRM_ACCESS         equ     038h
NVRM_ACCESS_MASK    equ     001h
NVRM_IDENT          equ     000h
NVRM_ADDRESS        equ     002h
NVRM_DATA           equ     004h
NVRM_DATA32         equ     006h
NVRM_SELECT_MASK    equ     006h        ; Mask for decode bits
NVRM_SELECT         equ     1           ; Least signifcant bit is 1


;----------------------------------------------------------------------------
unlock_extension_regs           macro

        mov     edx,SEQ_INDEX           ; EDX -> SR index register
        in      al,dx                   ; get current SR index
        ror     eax,8                   ; save it

        mov     ax,05706h               ; this will unlock the extension regs
        out     dx,ax                   ;
;
        rol     eax,8                   ; get the index
        out     dx,al                   ; restore SR index
        endm


;
;The PerVMData structure.  This structure represents the our reserved space
;in the so-called CB data structure.  The CB data structure is created for
;each VM started in the system.  We can therefore, save states that we need
;to remember on a per-VM basis.  This data will vary for every mini-VDD.
;The NVidia chipsets requires a few registers states for its extension registers
;to be saved.
;
PerVMData       struc
        Latch0                  db      ?               ; Graphics controller latches
        Latch1                  db      ?
        Latch2                  db      ?
        Latch3                  db      ?
        SeqReg06                db      ?
IFDEF NVIDIA_SPECIFIC
        EXT_Repaint0NV          db      ?               ; CRTC[19]
        EXT_Repaint1NV          db      ?               ; CRTC[1A]
        EXT_Repaint1NVSet       db      ?               ; CRTC[1A] Set?
        EXT_FifoBurst           db      ?               ; CRTC[1B]
        EXT_Repaint3NV          db      ?               ; CRTC[1C]
ENDIF ; NVIDIA_SPECIFIC
        EXT_WriteBank           db      ?               ; CRTC[1D]
        EXT_ReadBank            db      ?               ; CRTC[1E]
        EXT_Unlock              db      ?               ; CRTC[1F]
IFDEF NVIDIA_SPECIFIC
        EXT_FifoLowWaterMark    db      ?               ; CRTC[20]
        EXT_ExtraBits           db      ?               ; CRTC[25]
        EXT_PixelFormat         db      ?               ; CRTC[28]
        EXT_TVDecimation        db      ?               ; CRTC[29]
        EXT_OverscanHigh        db      ?               ; CRTC[2A]
        EXT_HorizontalExtra     db      ?               ; CRTC[2D]
ENDIF ; NVIDIA_SPECIFIC
        EXT_HWCursorAddress0    db      ?               ; CRTC[30]
        EXT_HWCursorAddress1    db      ?               ; CRTC[31]
IFDEF NVIDIA_SPECIFIC
        EXT_VideoWindow0        db      ?               ; CRTC[32]
        EXT_VideoWindow1        db      ?               ; CRTC[33]
ENDIF ; NVIDIA_SPECIFIC
IFDEF NVIDIA_COMMON
        EXT_RealModeAccess      db      ?               ; CRTC[38]
ENDIF ; NVIDIA_COMMON
IFDEF NVIDIA_SPECIFIC
        EXT_Interlace           db      ?               ; CRTC[39]
ENDIF ; NVIDIA_SPECIFIC
            EXT_CanopusTVReg    db      ?               ; Canopus TV Flag register (which ever it is. )
IFDEF NVIDIA_COMMON
        EXT_RealModeAddress     dd      ?               ; Real mode address
        EXT_RealModeData32      dw      ?               ; Latched word for real mode data
;        EXT_VpllM               db      ?               ; VPLL M
;        EXT_VpllN               db      ?               ; VPLL N
;        EXT_VpllP               db      ?               ; VPLL P
        EXT_NV_VPLL_COEFF       dd      ?               ; VPLL Coefficients        
        EXT_NV_PRAMDAC          dd      ?               ; NV_PRAMDAC_PLL_COEFF_SELECT
        EXT_NV_PDAC_GEN_CTRL    dd      ?               ; NV_PRAMDAC_GENERAL_CONTROL
        EXT_NV_PFB_CONFIG_0     dd      ?               ; NV_PFB_CONFIG_0
        EXT_NV_VPLL2_COEFF      dd      ?               ; VPLL Coefficients        
ENDIF ; NVIDIA_COMMON
        Majic                   dd      ?               ; Used for debugging
        StdCRTC                 db      25 dup (?)      ; CRTC 0-18h
;;;;;;;;SR1                     db      ?               ; Sequencer 1
        GoingFullscreen         db      ?               ; set in PostHiresToVGA, cleared in RestoreRegs
IFDEF NVIDIA_SPECIFIC
        EXT_HeadOwner           db      ?               ; CRTC[44]
ENDIF ; NVIDIA_SPECIFIC
PerVMData       ends
;
;FALSE           equ     TRUE
TRUE            equ     (NOT FALSE)
;
PCI_MECH1_CNT           record  PCI_ENABLE_CONFIG1:1, PCI_RESA1:7, PCI_BUS1:8, \
                                PCI_DEVICE1:5, PCI_FUNCTION1:3, PCI_DWORD1:6, PCI_RESB1:2
;PCI_DEVICE_MAX          equ     32      
;PCI_DEVICE_LOW_BIT      equ     11      
PCI_MECH1_ADDR          equ     00CF8h
PCI_MECH1_DATA          equ     00CFCh
;
PCI_MECH2_CSE         equ     00CF8h
PCI_MECH2_FORWARD     equ     00CFAh
PCI_MECH2_BASE        equ     0C000h
PCI_MECH2_ADDR          record  PCI_RESA2:4, PCI_DEVICE2:4, PCI_DWORD2:6, PCI_RESB2:2
PCI_MECH2_CNT           record  PCI_ENABLE_CONFIG2:4, PCI_FUNCTION2:3, PCI_SCE2:1
PCI_MECH2_KEY_ENABLE    equ     00Fh
;PCI_ENABLE_CONFIG_MASK  equ     080000000h
;PCI_VENDOR_ID_MASK      equ     0FFFFh
;PCI_BASE_ADDR_MASK      equ     0FFFFFFF0h
;PCI_FUNC_LOW_BIT        equ     8
PCI_CONF_00             record  PCI_DEV_ID:16, PCI_VENDOR_ID:16
PCI_CONF_01             record  PCI_STATUS:16, PCI_COMMAND:16
PCI_CONF_02             record  PCI_CLASS:24, PCI_REV:8
PCI_CONF_03             record  PCI_BIST:8, PCI_HEADER_TYPE:8, PCI_LATENCY:8, PCI_CACHE_LINE:8
PCI_CONF_11             record  PCI_SUB_ID:16, PCI_SUB_VENDOR_ID:16
PCI_CONF_15             record  PCI_MAX_LAT:8, PCI_MIN_GNT:8, PCI_INT_PIN:8, PCI_INT_LINE:8
PCI_BAR                 record  PCI_ADDR:28, PCI_PREFETCH:1, PCI_TYPE:2, PCI_MEM:1
PCI_CMD                 record  PCI_CMD_RES:6, PCI_CMD_FAST_BB:1, PCI_CMD_SERR:1, PCI_CMD_WAIT_CC:1, \
                                PCI_CMD_PARITY:1, PCI_CMD_PALETTE:1, PCI_CMD_MWI:1, PCI_CMD_SCM:1, \
                                PCI_CMD_MASTER:1, PCI_CMD_MEM:1, PCI_CMD_IO:1
;
PCI_DWORD_0             equ     0
PCI_DWORD_1             equ     1
PCI_DWORD_2             equ     2
PCI_DWORD_3             equ     3
PCI_DWORD_4             equ     4
PCI_DWORD_5             equ     5
PCI_DWORD_6             equ     6
PCI_DWORD_7             equ     7
PCI_DWORD_8             equ     8
PCI_DWORD_9             equ     9
PCI_DWORD_10            equ     10
PCI_DWORD_11            equ     11
PCI_DWORD_12            equ     12
PCI_DWORD_13            equ     13
PCI_DWORD_14            equ     14
PCI_DWORD_15            equ     15
;
NV_DEFAULT              equ     0CC000000h
;
PCI_FUNCTION_ID         equ     0B1h
PCI_BIOS_PRESENT        equ     001h
FIND_PCI_DEVICE         equ     002h
FIND_PCI_CLASS_CODE     equ     003h
GENERATE_SPECIAL_CYCLE  equ     006h
READ_CONFIG_BYTE        equ     008h
READ_CONFIG_WORD        equ     009h
READ_CONFIG_DWORD       equ     00Ah
WRITE_CONFIG_BYTE       equ     00Bh
WRITE_CONFIG_WORD       equ     00Ch
WRITE_CONFIG_DWORD      equ     00Dh
;
PCI_SUCCESSFUL          equ     000h
PCI_FUNC_NOT_SUPPORTED  equ     081h
PCI_BAD_VENDOR_ID       equ     083h
PCI_DEVICE_NOT_FOUND    equ     086h
PCI_BAD_REGISTER_NUMBER equ     087h
;
RefreshRateStruct       struc
VetricalRefreshRate             dd      ?               ; Vertical refresh rate in Hz
HorizontalRefreshRate           dd      ?               ; Horizontal refresh rate in Hz
crtcInfoBlockForced             CRTCInfoBlock   <>      ; CRTCInfoBlock        
RefreshRateStruct       ends
;
PRefreshRateStruct      typedef ptr RefreshRateStruct
;
; Clock rates that must be in the NVidia BIOS clock table
DC_031_500              equ     3150
DC_025_175              equ     2518
DC_049_500              equ     4950
DC_050_000              equ     5000
DC_040_000              equ     4000
DC_036_000              equ     3600
DC_078_750              equ     7875
DC_075_000              equ     7500
DC_065_000              equ     6500
DC_044_900              equ     4490
DC_135_000              equ     13500
DC_108_500              equ     10850

;;;;typedef struct def_rmusrapi_struct {
;;;;  U032  function;       
;;;;  U032  device; 
;;;;} RMUSRAPI_STRUCT,*PRMUSRAPI_STRUCT;
minivdd_CALLRM_rmusrapi_Struct struct
  rmusrapi_function           dd      0
  rmusrapi_device             dd      0
minivdd_CALLRM_rmusrapi_Struct ends
DEFAULT_PRIMARY_HDEV          equ 1
DISPLAY_TYPE_MONITOR          equ 0
DISPLAY_TYPE_TV               equ 1
DISPLAY_TYPE_DUALSURFACE      equ 2
DISPLAY_TYPE_FLAT_PANEL       equ 3
