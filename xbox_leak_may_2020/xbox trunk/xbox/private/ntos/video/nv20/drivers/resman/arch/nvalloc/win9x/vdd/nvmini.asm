;       (C) Copyright NVIDIA Corporation Inc., 1996,1997. All rights reserved.
;       Portions Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.
;
;       NVidia Mini-VDD
;
;       Written by:     Mark Stephen Krueger
;       Date:           10/1/96
;               JohnH 10/15/97  spun off version for release 2
;
;       $Revision: 6 $
;       $Date: 6/04/98 6:08p $
;       $Logfile: /resman.050/win96/vdd/nv4.asm $
;       $Log: /resman.050/win96/vdd/nv4.asm $
;
; 6     6/04/98 6:08p Jhinman
; Added WaitTicks call to POST_HIRESTOVGA and POST_VGATOHIRES.
;
; 5     6/02/98 3:49p Schaefer
; partway complete change to enable another level of indirection with the
; setcallback system.
;
; -C-
;
; 4     5/28/98 4:47p Schaefer
; * added callback routines for VBI and Image field completion
;
; 3     5/28/98 10:36a Schaefer
; add functionality for VPE support,  moved data structres into Ring0
; space
;
; 2     5/14/98 4:35p Schaefer
; updated to include KMVT functions in VDD
;
; 1     5/07/98 1:00p Jhinman
; Name change.
;
; 1     5/05/98 5:14p Jhinman
; Gave the mini-VDD a non-chip-specific name.
;
; 28    4/29/98 11:51p Kevink
; Cleaned up NV4 naming (again)
;
; 27    4/29/98 11:14p Kevink
; Fixed an IFDEF
;
; 26    4/28/98 6:20p Kevink
; Added NV4 naming
;
; 24    4/23/98 8:00p Jhinman
; Set DisplayEnabledFlag in PostCRTCModeChange, in case GetVDDBank does
; not get called.
;
; 23    4/16/98 9:07p Jhinman
; Set displayenabledflag in DisplayDriverDisabling and check it in
; RestoreRegisters.
;
; 22    4/01/98 12:09p Jhinman
; Move system vm handle to ebx before save registers/save message mode
; state.
;
; 21    3/30/98 8:40p Jvito
; Don't bother with the cursor HW address registers on a restore.
;
; 20    3/10/98 10:36a Jhinman
; Added comment which fixes "line too long" error.
;
; 19    3/09/98 5:49p Jhinman
; Do the saveRegisters/SaveMessageModeState at pnp_new_devnode.
;
; 18    2/26/98 5:08p Jhinman
; Pass the address of PostCRTCModeChange to the resource manager, so it
; can notify us when a mode change has taken place, and we will save
; registers.
;
; 17    2/24/98 11:22p Jvito
; Code changes to allow the MiniVDD to monitor mode changes in the RM.
;
; 16    2/18/98 3:58p Jvito
; Exit the minivdd_restoreregisters routine if entered by the system.
;
; 15    2/13/98 2:27p Jhinman
; Add device number to the call to resource manager to map device. Add
; amount of video RAM to GetTotalVRAMsize. Add SaveRegisters to
; PostCRTCModeChange.
;
; 14    1/14/98 3:05p Jhinman
; Handle SYSTEM_EXIT message.
;
; 13    1/09/98 2:01p Jhinman
; Split miniVDD from the resource manager again. Load RM and communicate
; with it through its API.
;
; 12    12/17/97 2:29p Jhinman
; Check for Windows 98 before hooking turnVGAon/off. Don't do FindNV3.
;
; 11    12/10/97 11:05a Jhinman
; Enable VGATurnOff/On.
;
; 10    12/02/97 6:10p Jhinman
; pnp_new_devnode: jmp to minivdd_pnp_done if error.
;
; 8     11/21/97 5:56p Jhinman
; Check addresses and IRQ are nonzero before we call rmConfigStart.
;
; 7     11/15/97 11:37a Jhinman
; Enable pnp_new_devnode, but do not hook the pnp handler. Get the config
; from config manager and call configStart in pnp_new_devnode.
;
; 6     11/07/97 6:44p Jhinman
; If P_N_P defined, do plug and play stuff.
;
; 5     10/30/97 6:44p Jhinman
; Changed the way miniVDD communicates the addresses to RM (nvAddr,
; nvPhys, etc. are global).
;
; 4     10/29/97 4:53p Waynek
; Add new Memphis MiniVDD entry points to allow the minivdd to turn off
; access to the VGA resources.
;
; 3     10/29/97 4:29p Jhinman
; Removed the ifdef UNIFY_VXDS and added some debug messages in Plug and
; Play handler.
;
; 2     10/27/97 3:07p Jhinman
; Unified miniVDD/resource manager.
;
;
page            ,132
title           NVidia NV3 Mini-VDD Support Functions
.386p
;

VESA20  equ 1   ; turn on fixup of VBE DDC functions: 
                ; 1) create 1.x EDID from a 2.0 EDID,
                ; 2) read from alternate device addresses (A6, A2, A0)
RM_GET_DDC_CAPS equ     1       ; let RM handle GET DDC CAPABILITES instead of the BIOS

;
.xlist
MINIVDD                 EQU             1               ; this must be defined for minivdd.inc

include         VMM.INC
include         SHELL.INC
include         VWIN32.INC                      ; used for IOCTL for KMVT
include                 VXDLDR.INC
include         DEBUG.INC
include         MINIVDD.INC
include         configmg.inc
include         VNVRMD.INC
include         V86MMGR.INC
include         NVDEF.INC
include         VBE30.INC
include         NVIDIA.INC
IFDEF NVIDIA_COMMON
include         NV_REF.INC
include         NV_MAC.INC
include         NV_SIM.INC
ENDIF ; NVIDIA_COMMON
include         SURFACES.INC
IFDEF NVPE
include         NVPEOS.INC
ENDIF
include         ..\..\..\..\..\sdk\nvidia\inc\rddapi.inc

IFDEF   DIOC
extrn           GetDDHAL:near
extrn           NvFillTable:near
extern          NVGetSetCallback:near
extern          NVSetVidTexSuf:near
ENDIF   ;DIOC

IFDEF NVPE
extrn           _nvpeVDDCmdHandler:near
extrn           _nvpeVDDClearState:near
extrn               _nvpeVDDInit:near
ENDIF

IFDEF   I2C
extrn           _vddI2COpen:near
extrn           _vddI2CAccess:near
NOCHANNEL       equ     -1
ENDIF   ;I2C

public                  MiniVDD_BroadcastHandler


; defines needed for Canopus TV extensions

TVB_VBE_FCT             equ 04f14h  ; VBE OEM function number
TVB_VBE_TV_GET          equ 00102h  ; sub function to get the TV flag
TVB_VBE_TV_SET          equ 00202h  ; sub function to set the TV flag
TVB_VBE_TV_CRTC         equ 00302h  ; sub function to get the TV flag CRTC index
TVF_WINDOWS             equ 040h    ; marker for our mini VDD
TVF_INACTIVE            equ 080h    ; no hardware initialisation (of the Chrontel chip)

CANOPUS_TV_REG                  equ             02bh    ;; TV flag register index

; defines needed for Advanced Power Management

WM_POWERBROADCAST                               EQU 218h

PBT_APMQUERYSUSPEND             EQU 0
PBT_APMQUERYSTANDBY             EQU 1

PBT_APMQUERYSUSPENDFAILED       EQU 2
PBT_APMQUERYSTANDBYFAILED       EQU 3

PBT_APMSUSPEND                  EQU 4
PBT_APMSTANDBY                  EQU 5

PBT_APMRESUMECRITICAL           EQU 6
PBT_APMRESUMESUSPEND            EQU 7
PBT_APMRESUMESTANDBY            EQU 8

PBTF_APMRESUMEFROMFAILURE       EQU 1

PBT_APMBATTERYLOW               EQU 9
PBT_APMPOWERSTATUSCHANGE        EQU 0Ah

PBT_APMOEMEVENT                 EQU 0Bh
PBT_APMRESUMEAUTOMATIC          EQU 12h

.list

;**************************************************************************************************************************
subttl          Virtual Device Declaration
page +
;       Then MiniVDD must have the UNDEFINED_DEVICE_ID
;       and it is a dynamic load VxD, so it cannot have a service table.
Declare_Virtual_Device  NVMINI, \
                        3,      \
                        1,      \
                        MiniVDD_Control,        \
                        UNDEFINED_DEVICE_ID,    \
                        VDD_INIT_ORDER,         \
                                                ,                                               \
                        ,                                       \
                        ,

;
;
;**************************************************************************************************************************
subttl          Initialization Data
page +
VxD_IDATA_SEG

VxD_IDATA_ENDS
;
;**************************************************************************************************************************
subttl          Locked Data Area
page +
VxD_LOCKED_DATA_SEG
PUBLIC  dwCallbacksSelector, pfnLockOffscreen, pfnUnlockOffscreen, pfnPowerOn, pfnPowerOff
dwCallbacksSelector     DD      0
pfnLockOffscreen        DD      0
pfnUnlockOffscreen      DD      0
pfnPowerOn              DD      0
pfnPowerOff             DD      0
pfnCurrentCall          DD      0

GetMonInfoCount         DD      -1
dwInt10EntryCount       DD      -1

PUBLIC  dwDisplayDevnodeHandle
dwDisplayDevnodeHandle  DD      0

PUBLIC  szEnumAndMore
szEnumAndMore           DB      "Enum\"
szAndMore               DB      128 DUP(0)
szAllowMightyModes      DB      "AllowMightyModes",0
dwAllowMightyModes      DD      0

PUBLIC  szLocalDisplayPath, szLocalDisplayPathEnd
szLocalDisplayPath      DB      "System\CurrentControlSet\Services\Class\"
szLocalDisplayPathEnd   DB      32 DUP(0)

PUBLIC  szDriver
szDriver        DB      "Driver",0

PUBLIC  szDisplayEndPath
szDisplayEndPath        DB      32 DUP(0)

ALIGN 4
PUBLIC  dwCBType, dwCBLen, hKey
dwCBType        DD      0
dwCBLen         DD      0
hKey            DD      0

EDID1_HEADER    DB      0,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0

; Need to rewrite ReenumerateDevNode routine
; when increase this number!!!
MAX_CRTCS       equ     2
LogicalDevice   DD      MAX_CRTCS DUP (0)

PUBLIC  pDDrawFullScreenDOSActive
pDDrawFullScreenDOSActive       DD      0
VxD_LOCKED_DATA_ENDS
;
;**************************************************************************************************************************
subttl          General Data Area
page +
VxD_DATA_SEG
;
IFDEF DEBUG_LOG
public  DebugOn
DebugOn                 dd      1               ;Should routines log debug info?
;
ENDIF ; DEBUG_LOG
public  WindowsVMHandle
WindowsVMHandle         dd      ?               ;init'd at Device_Init
;
public  PlanarStateID, MessageModeID
PlanarStateID           dd      ?               ;init'd at RegisterPlanarState
MessageModeID           dd      ?               ;init'd at RegisterMessageMode
;
public  OSVersion
OSVersion               dw      ?               ;init'd at Device_Init

public  TotalMemorySize
TotalMemorySize         dd      ?               ;init'd at Sys_Critical_Init
;
public  Vid_PhysA0000
Vid_PhysA0000           dd      0               ;
;
public  OurCBDataPointer
OurCBDataPointer        dd      ?               ;init'd at Device_Init
;
public  MessageModeCBData
MessageModeCBData       db      size PerVMData dup(0)
;
public  ForcedPlanarCBData
ForcedPlanarCBData      db      size PerVMData dup(0)
;
public  MemoryMappedSelector
MemoryMappedSelector    dd      0               ;init'd at RegisterDisplayDriver
;
public  ChipID                                  ;NVidia specific variable!
ChipType                db      3               ;init'd at pnp_new_devnode
;

public  ModeChangeFlags
ModeChangeFlags         db      ?               ;=FFH when in BIOS mode change

public  DosBoxActive
DosBoxActive            db      ?
; Equate used by for API to the RM
NVRM_API_SIGNAL_MODE_SAVE       equ     1000+71
;
public  DisplayEnabledFlag
DisplayEnabledFlag      db      0               ;=FFH when display in HiRes mode
;
public  SuperVGAModeFlag
SuperVGAModeFlag        db      0               ;=FFH when NOT running VGA mode
;
public  MsgModeInitDone
MsgModeInitDone         db      0               ;=FFH when collecting states
;
public  VGAModeFlag
VGAModeFlag             db      0               ;=FFh when in 16 color mode (VGA driver)
;
public  InRefreshSetMode
InRefreshSetMode        db      0               ;=FFH when in int 10 already

SeqIdx                  db      ?
LockValue               db      ?

ResManAPI               dd      0                               ; NVRM API entry point
;
nvPhys                  dd      0                               ; physical address given us by Configuration Manager
fbPhys                  dd      0                               ;         "
fbLength                dd      0               ; 16 or 32 MB
nvIRQ                   dd      0                               ; physical irq       "
nvAddr                  dd      0                               ; linear address used everywhere to access chip
fbAddr                  dd      0
nvCustomer              dd      0                               ; bitmask of current customer support
NVCUSTOMER_CANOPUS      EQU     1
NVCUSTOMER_TOSHIBA      EQU     2
fbBuffer                dd      0
fpDDC                   dd      0
NumUnits                dd      0               ; number of "units" or heads
cursor_pos              dw      0
ChipID                  dd      0                               ; master control boot register
hDev                    dd      0
NVRM_String             db      'NVCORE.VXD',0   ; filename of the resource manager
DevCtlRet               dd      0               ; Dev IOCTL function ret val
ACPI_state              db      0               ; ACPI state 0-3
configStartDone         db      0               ; Flag to protect duplicate call to CONFIG_START
public  DoNotSimulate
DoNotSimulate           db      0
RestoreStdCrtc          db      0
DisplayChanged          db      0
DDC_Caps                dw      0       ; Save DDC Caps
BuffOff                 dw      0       ; V86 buffer passed in to VESASupport
BuffSeg                 dw      0       ;  "
Edid2Map                dw      0
DetTimOff               dw      0
DDCAddrFake             dw      0       ; DDC address map
pEdid1                  dd      0       ; pointer to caller's 128 byte EDID buffer
pEdid2                  dd      0       ; pointer to V86 buffer alloc'd for 256 byte EDID
TempEdidBufOff          dw      0       ; real mode address of our EDID buffer
TempEdidBufSeg          dw      0       ;
EdidAddress             db      0       ; Save device address when retrying EDID reads
DontRecurse             db      0       ; when != just pass down
nDetTim                 db      0
numDevice               db      0       ; number of devices = number of times map config called

; Canopus globals
InFullScreen            db      0
bCanopusTVReg           db      0

; resource manager parameter passing structure.
minivdd_RM_Parameter_Struct struct
  callrm_device_handle        dd      0
  callrm_modechange_callback  dd      0
  callrm_irq_bus              dd      0
  callrm_fbphys               dd      0
  callrm_nvphys               dd      0
  callrm_registry_path        dd      0
minivdd_RM_Parameter_Struct ends
callrm_struct minivdd_RM_Parameter_Struct <>
callrm_rmusrapi minivdd_CALLRM_rmusrapi_Struct <>

;
public  DisplayInfoStructure, refreshRates
DisplayInfoStructure    DISPLAYINFO     <>
;
refreshRates    PRefreshRateStruct Refresh640,Refresh800,Refresh1024,Refresh1280
;
Refresh640      RefreshRateStruct<75,37,<0840,0656,0656+064,0500,0481,0481+3,'-','-',0,DC_031_500,7500>>
                RefreshRateStruct<60,31,<0800,0656,0656+096,0525,0490,0490+2,'-','-',0,DC_025_175,6000>>
                dd      0
;
Refresh800      RefreshRateStruct<75,46,<1056,0816,0816+080,0625,0601,0601+3,'+','+',0,DC_049_500,7500>>
                RefreshRateStruct<72,48,<1040,0856,0856+120,0666,0637,0637+6,'+','+',0,DC_050_000,7200>>
                RefreshRateStruct<60,37,<1056,0840,0840+128,0628,0601,0601+4,'+','+',0,DC_040_000,6000>>
                RefreshRateStruct<56,35,<1024,0824,0824+072,0625,0601,0601+2,'+','+',0,DC_036_000,5600>>
                dd      0
;
Refresh1024     RefreshRateStruct<75,60,<1312,1040,1040+096,0800,0769,0769+3,'+','+',0,DC_078_750,7500>>
                RefreshRateStruct<70,56,<1328,1048,1048+136,0806,0771,0771+6,'-','-',0,DC_075_000,7000>>
                RefreshRateStruct<60,48,<1344,1048,1048+136,0806,0771,0771+6,'-','-',0,DC_065_000,6000>>
                RefreshRateStruct<43,35,<1264,1032,1032+176,0817,0768,0768+4,'+','+',1,DC_044_900,4300>> ; Interlaced
                dd      0
;
Refresh1280     RefreshRateStruct<60,64,<1696,1344,1344+016,1066,1025,1025+3,'+','+',0,DC_108_500,6000>>
                RefreshRateStruct<75,79,<1688,1296,1296+144,1066,1025,1025+3,'+','+',0,DC_135_000,7500>>
                dd      0
;
; EDID 1.0 template
; for creating a phony version 1 EDID when the actual EDID is version 2
EdidTemp        db      0,0ffh,0ffh,0ffh,0ffh,0ffh,0ffh,0   ; header
                db      0,0,0,0,0,0,0,0,1,10                ; vendor/product ID
                db      1,0                                 ; EDID version
                db      0ch,20h,18h,0beh,0e8h               ; display parameters
                db      0e1h,062h,09eh,055h,04ah,098h,027h,012h,048h,04fh ; color characteristics
                db      3   dup(0)                          ; established timings
                db      16  dup(0)                          ; standard timing
                db      72  dup(0)                          ; detailed timing
                db      0                                   ; extension flag
                db      0                                   ; checksum
; end of EDID

EDID1_VENDOR    equ     8
EDID1_SIZE      equ     128
EDID1_DET_TIMING        equ 36h     ;offset to detailed timing

EDID2_VENDOR    equ     1
EDID2_SIZE      equ     256
; EDID 2 offsets and bit defs
EDID2_MAP               equ 7eh     ;offset to map
EDID2_TIMING_START      equ 80h     ;offset to timing
EDID2_LUM_TABLE_EXIST   equ 20h
EDID2_LUM_ENTRIES       equ 1fh
EDID2_LUMX3             equ 80h
EDID2_RL                equ 1ch
EDID2_RL_SHIFT          equ 2
EDID2_DRL               equ 3
EDID2_TIMING_CODES      equ 0f8h
EDID2_TC_SHIFT          equ 3
EDID2_DET_TIMINGS       equ 07h

; DDC bit defs
DDC_ADDR_A0             equ 03h     ; DDC address is A0 (DDC 1 or DDC 2)
DDC_ADDR_A2             equ 08h     ; DDC address is A2 (DDC 2)
DDC_ADDR_A6             equ 10h     ; DDC address is A6 (DDC 2)
DDC_SUBF_1              equ 1       ; DDC read EDID at A0
DDC_SUBF_3              equ 3       ; DDC read EDID at A2
DDC_SUBF_4              equ 4       ; DDC read EDID at A6


VxD_DATA_ENDS
;
;
;**************************************************************************************************************************
subttl          Device Initialization
page +
VxD_ICODE_SEG

;
public  MiniVDD_Dynamic_Init
BeginProc MiniVDD_Dynamic_Init

;
;Entry:
;       EBX contains the VM handle of the Windows VM.
;Exit:
;       If success, return NC.
;       If failure, return CY.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_Dyanmic_Init", DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        push    ebx                     ;save Windows VM handle in EBX
        push    ebp
        mov     WindowsVMHandle,ebx     ;save the Windows VM handle
        mov     ModeChangeFlags,0       ;initialize this
        mov     DosBoxActive,0
;ifdef  SEPARATE_RM
;
;       We are the primary display
;       load NV ResMan
;
        mov             edx, OFFSET32 NVRM_String
        mov             eax, VXDLDR_INIT_DEVICE
        VxDcall VXDLDR_LoadDevice
        jc              MVDIErrorExit                   ;failed, nothing we can do

;       returns DDB address in eax and device handle in edx
;       save the API entry
        mov             eax, [eax].DDB_PM_API_Proc
        mov             ResManAPI, eax
;endif  ;SEPARATE_RM

;       Correct the string for multihead configuration
;       Change %s\Unit%d --> %s\U%d
        push    ecx
        push    edx
        push    edi
        mov     eax,0ah
        xor     edi,edi
        VMMCall Get_DDB
        or      ecx,ecx
        jz      @F
        lea     edi,[ecx+18h]
        mov     edx,ASSERT_RANGE_NO_DEBUG OR ASSERT_RANGE_NULL_BAD
        VMMCall _Assert_Range,<edi,4,0,0,edx>
        or      eax,eax
        jz      @F
        mov     edi,[edi]
        lea     edi,[edi+13fch]
        VMMCall _Assert_Range,<edi,4,2574696eh,0,edx>
        or      eax,eax
        jz      @F
        mov     ecx,6425h               ; '%d'
        mov     [edi],ecx
@@:
        ; Initialize LogicalDevice structure.
        lea     edi,LogicalDevice
        sub     ecx,ecx

@@:
        mov     [edi],ecx
        inc     ecx
        add     edi,4
        cmp     ecx,MAX_CRTCS
        jb      @B

        pop     edi
        pop     edx
        pop     ecx
;
;Get the linear address of physical A0000H addressable for 64K:
;
        VMMCall _MapPhysToLinear,<0A0000h,10000h,0>
        cmp     eax,-1                  ;was there an error?
        je      MVDIErrorExit           ;yes, better not load!
        mov     Vid_PhysA0000,eax       ;save this off!

;       clear VPE's global state
        call _nvpeVDDClearState

;       initialize WDM prescale values
        call _nvpeVDDInit
;
public  MVDIGetSpecialVMs
MVDIGetSpecialVMs:
;
;There are two special VM states that the "main" VDD establishes.  The
;first is the planar state which is simply a state that the "main" VDD
;restores to establish the VGA 4 plane mode.  When we restore the states
;at MiniVDD_RestoreRegisterState, we check for the special identifier which
;flags that we're restoring this state.  If we find that we are restoring
;the planar state, we have the opportunity to special case the register
;state restore.
;
;Similarly, there is a special state called "Message Mode".  This is the
;state when the Shell VxD is displaying a full-screen message (usually
;with a blue background) telling the user of a serious anomaly in the
;running state of Windows.  We also retrieve the special VM handle
;for the "Message Mode" state so we can handle it special too if needed.
;
        VxDCall VDD_Get_Special_VM_IDs  ;go get special VM information
        mov     PlanarStateID,esi       ;save off returned identifiers
        mov     MessageModeID,edi       ;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    ":PlanarStateID:%08lX", esi, DEBLEVELMAX
                Debug_Printf    ":MessageModeID:%08lX", edi, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
;
public  MVDIDispatch
MVDIDispatch:
;
;We want to allocate space in the per-VM CB data structure.  This will
;be used by the mini-VDD to store per-VM data relating to states of registers.
;
        VMMCall _Allocate_Device_CB_Area,<<size PerVMData>,0>
        mov     OurCBDataPointer,eax    ;save offset of our VxD's area
        or      eax,eax                 ;was call sucessful?
        jz      MVDIErrorExit           ;nope, leave fatally!
;
;The "master" VDD (VDD.386) contains all of the routines and procedures
;that are necessary to virtualize a standard VGA environment on high
;resolution super-VGA cards.  This "mini-VDD" provides some simple
;services which provide support which are peculiar to the chipset
;that we're providing support for.
;
;We must register the entry point addresses of our chipset-dependent
;routines with the "master" VDD.  To do this, we call a service in the
;"master" VDD, which returns the address of a dispatch table of the
;which is an array of the format:
;
;               dd      Address of support routine in this mini-VDD.
;                       This address will be initialized to 0 by the "master"
;                       VDD.  If we need to implement this functionality,
;                       we fill in the address of our routine.
;
;Then, when the "master" VDD needs to call this function, it'll see if we've
;filled in the address field in for the function's entry and will call our
;routine if the address is filled in.  Otherwise, the VDD will skip the call
;and continue on its way without calling us.
;
;The following function calls the "master" VDD and returns a pointer to the
;dispatch table documented above in EDI and the number of functions
;supported in ECX. If the number of functions returned by the "master" VDD
;is less than what we think it is, we return an error and don't allow
;Windows to continue running.
;
        VxDCall VDD_Get_Mini_Dispatch_Table
        cmp     ecx,NBR_MINI_VDD_FUNCTIONS      ;perform a cursory version check
        jb      MVDIErrorExit                   ;oops, versions don't match!
;
public  MVDIFillInTable
MVDIFillInTable:
;
;Fill in the addresses of all the functions that we need to handle in this
;mini-VDD in the table provided by the "master" VDD whose address is
;contained in EDI.  Note that if you do not need to support a function,
;you do not need to fill in the dispatch table entry for that function.
;If you do not fill in an entry, the "master" VDD won't try to call
;this mini-VDD to support the function.  It'll just handle it in a
;default manner.
;
        MiniVDDDispatch REGISTER_DISPLAY_DRIVER,RegisterDisplayDriver
; if we don't support vitualizing planar mode, we don't need the following 2 functions:
        MiniVDDDispatch GET_VDD_BANK,GetVDDBank
        MiniVDDDispatch SET_VDD_BANK,SetVDDBank
        MiniVDDDispatch RESET_BANK,ResetBank
;We still need these routines for ourselves in RestoreLatches, but the
;main VDD should never call them because we have SaveLatches and
;RestoreLatches.
        MiniVDDDispatch SET_LATCH_BANK,SetLatchBank
        MiniVDDDispatch RESET_LATCH_BANK,ResetLatchBank
;        MiniVDDDispatch SAVE_LATCHES,SaveLatches
;        MiniVDDDispatch RESTORE_LATCHES,RestoreLatches
        MiniVDDDispatch PRE_HIRES_TO_VGA,PreHiResToVGA
        MiniVDDDispatch POST_HIRES_TO_VGA,PostHiResToVGA
        MiniVDDDispatch PRE_VGA_TO_HIRES,PreVGAToHiRes
        MiniVDDDispatch POST_VGA_TO_HIRES,PostVGAToHiRes
        MiniVDDDispatch SAVE_REGISTERS,SaveRegisters
        MiniVDDDispatch RESTORE_REGISTERS,RestoreRegisters
        MiniVDDDispatch DISPLAY_DRIVER_DISABLING,DisplayDriverDisabling
        MiniVDDDispatch GET_TOTAL_VRAM_SIZE,GetTotalVRAMSize
        MiniVDDDispatch GET_CURRENT_BANK_WRITE,GetCurrentBankWrite
        MiniVDDDispatch GET_CURRENT_BANK_READ,GetCurrentBankRead
        MiniVDDDispatch GET_BANK_SIZE,GetBankSize
        MiniVDDDispatch SET_BANK,SetBank
        MiniVDDDispatch PRE_HIRES_SAVE_RESTORE,PreHiResSaveRestore
        MiniVDDDispatch POST_HIRES_SAVE_RESTORE,PostHiResSaveRestore
        MiniVDDDispatch CHECK_SCREEN_SWITCH_OK,CheckScreenSwitchOK
        MiniVDDDispatch GET_CHIP_ID,GetChipID
        MiniVDDDispatch VIRTUALIZE_SEQUENCER_OUT,VirtSeqOut
        MiniVDDDispatch VIRTUALIZE_SEQUENCER_IN,VirtSeqIn
        MiniVDDDispatch ENABLE_TRAPS,EnableTraps
        MiniVDDDispatch DISABLE_TRAPS,DisableTraps
        MiniVDDDispatch MAKE_HARDWARE_NOT_BUSY,MakeHardwareNotBusy
        MiniVDDDispatch VIRTUALIZE_CRTC_OUT,VirtCRTCOut
        MiniVDDDispatch VIRTUALIZE_CRTC_IN,VirtCRTCIn
        MiniVDDDispatch PRE_CRTC_MODE_CHANGE,PreCRTCModeChange
        MiniVDDDispatch POST_CRTC_MODE_CHANGE,PostCRTCModeChange
        MiniVDDDispatch SAVE_FORCED_PLANAR_STATE,SaveForcedPlanarState
        MiniVDDDispatch SAVE_MESSAGE_MODE_STATE,SaveMessageModeState
        MiniVDDDispatch VESA_SUPPORT,VESASupport
        MiniVDDDispatch GET_MONITOR_INFO,GetMonitorInfo
        ;===============================================
        ;
        ; new Win98 functions...
        ;
        VMMcall Get_VMM_Version
        mov OSVersion,ax        ; save OS version
        jc  PreWin98
        cmp   ax,VERS_WIN98
        jb    PreWin98
;         cmp   ax,045ah
;         jb    PreMillen
IFDEF   I2C
        ;I2C functions
          MiniVDDDispatch I2C_OPEN, I2COpen
          MiniVDDDispatch I2C_ACCESS, I2CAccess
ENDIF   ;I2C
        ;These are required for multi-display support
          MiniVDDDispatch TURN_VGA_OFF,TurnVgaOff
          MiniVDDDispatch TURN_VGA_ON,TurnVgaOn
        ;This is required if the device supports multiple displays
          MiniVDDDispatch GET_NUM_UNITS,GetNumUnits
        ;These are for power management. We don't need the monitor functions, since Windows will use VESA calls
        ; if we don't hook them.
;ifdef   ON_NOW
        pushad
        mov     eax,OFFSET32 DisplayInfoStructure
        mov     ecx,SIZE DISPLAYINFO    ;pass size in ECX, address in EAX
        mov     [eax].diHdrSize,cx      ;fill in this entry
        push    eax                     ;save EAX --> DisplayInfoStructure
        VxDCall VDD_Get_DISPLAYINFO     ;get information from the VDD
        pop     edi                     ;restore EAX --> DisplayInfoStructure
        mov     VGAModeFlag,0
        cmp     [edi].diBpp,4           ;16 color?
        je      SkipAdapterPM
        
        popad

        MiniVDDDispatch SET_MONITOR_POWER_STATE,SetMonitorPowerState
        MiniVDDDispatch GET_MONITOR_POWER_STATE_CAPS,GetMonitorPowerStateCaps
        MiniVDDDispatch SET_ADAPTER_POWER_STATE,SetAdapterPowerState
        MiniVDDDispatch GET_ADAPTER_POWER_STATE_CAPS,GetAdapterPowerStateCaps
        jmp     PreWin98

SkipAdapterPM:
        popad
;endif
PreWin98:
        ;===============================================

;
public  MVDI_SetupPortTrapping
MVDI_SetupPortTrapping:
;Trapping ports 3D0-3D3 is required on Win95 because it splits word accesses into bytes.
;Our HW cannot deal with bytes accesses, so I trap the access and reconstruct byte I/O
;into word I/O. If the system VM is the owner, it's windowed DOS, and I do not let the I/O thru.
;Win95 gives us system_exit, sys_vm_terminate, etc. so when windows exits, we let the I/O thru
;even if it is the system VM. However, Win98 & later OS's don't give us the sys_VM_terminate msg.
;Since we don't have a means to detect system exit, we better not trap. Fortunately, these OS's
;also don't split up the I/O.

        mov   ax,OSVersion
        cmp   ax,VERS_WIN98     
        jae   @f                

;
;Now comes the hard part (conceptually).  We must call the "master" VDD to
;setup port trapping for any port that the Windows display driver would
;write to (or read from) in order to draw onto the display when it's
;running in Windows Hi-Res mode.  For example, on the IBM 8514/A display
;card, the hardware BLTer is used to draw onto the screen.  Ports such as
;9AE8H, E2E8H, BEE8H are used on the 8514/A to perform the drawing.  The
;VDD "system" must set I/O port traps on these registers so that we are
;informed that the Windows display driver is writing or reading these
;ports. Only set traps on those ports which the display driver would
;write or read in the process of drawing.
;
; On NV chips, the CRTC extended registers are used, along with the linear
; frame buffer and the 64KB memory mapped channel to the NV chip FIFO.
; The display driver will register the channel and linear frame buffer, but
; we have to register the NV VGA real mode access registers that the BIOS
; uses. These remained trapped while Windows is running, but our port simulator
; routine checks whether it has been called from the Windows VM (in which case it ignores
; the trap) or someone else (it accesses the register).
IFDEF NVIDIA_COMMON
        mov     edx,003D0h              ;register port 003D0h
        mov     ecx,OFFSET32 MiniVDD_VirtualRealModeAccess
        VxDCall VDD_Takeover_VGA_Port
        mov     edx,003D1h              ;register port 003D1h
        mov     ecx,OFFSET32 MiniVDD_VirtualRealModeAccess
        VxDCall VDD_Takeover_VGA_Port
        mov     edx,003D2h              ;register port 003D2h
        mov     ecx,OFFSET32 MiniVDD_VirtualRealModeAccess
        VxDCall VDD_Takeover_VGA_Port
        mov     edx,003D3h              ;register port 003D3h
        mov     ecx,OFFSET32 MiniVDD_VirtualRealModeAccess
        VxDCall VDD_Takeover_VGA_Port
ENDIF ; NVIDIA_COMMON
@@:
        mov             eax, OFFSET32 MiniVDD_BroadcastHandler
        VxDcall _SHELL_HookSystemBroadcast, < eax, 0, 0 >

        ; Hook Int 10 so we can intercept the DDC calls
        mov     eax, 10h
        mov     esi, OFFSET32 MiniVDD_Int10Handler
        VxDcall Hook_V86_Int_Chain
;
; CANOPUS SUPPORT - detect if TV interface is present
;
;   Because we might need to change the CRTC index register we use to support TV, we
;       will get it from the bios and dynamicly handle the one we get.
MVDI_GetCanopusTVIndex:
        push    bx                      ;--- save regs

        mov ax, TVB_VBE_FCT             ; VBE OEM function
        mov     bx, TVB_VBE_TV_CRTC     ; get CRTC index to flag register
        push    010h                    ; call int 0x10
        VMMCall Exec_VxD_Int            ; execute interrupt

        cmp ax, 0004fh                  ; success ???
        je  MVDI_SaveTVIndex            ; yes, go on and save index
        xor bl, bl                      ; just set it to zero to mark, that we don't support it
MVDI_SaveTVIndex:
        mov bCanopusTVReg, bl           ; just save the value

        pop bx                          ;--- restore regs

;
MVDIGoodExit:
        clc                             ;return success
        jmp     MVDIExit                ;
;
MVDIErrorExit:
        stc
;
MVDIExit:
        pop     ebp
        pop     ebx                     ;restore Windows VM handle to EBX
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret
EndProc MiniVDD_Dynamic_Init
;

; Imhoff - Message handler for broadcast messages... We are interested in
; the WM_POWERBROADCAST message in order to do some kind of simple blanking
; of the monitor during suspend/standby... This addresses a problem we have seen
; under Win95 OSR2 where the monitor does not get blanked at all...

; JohnH - besides blanking the screen, which we do in the RM, this also prevents a problem on the Dell Optiplex system:
; a few seconds after going into Suspend, we get a Resume message. Adding this handler prevents us from leaving Suspend
; too soon.
; Apparently the sequence is this: RM gets Suspend (VxD Control message). Mini-VDD gets Suspend (System message).
; RM gets Resume (Control message). Mini-VDD gets another Suspend (System message). The second System Suspend message
; lets us blank the screen again after the RM Resume turned on the screen, so we will suspend until activity causes us
; to get a legitimate Resume.
; This may be a bug in APM in one of the other device's drivers on the Dell Optiplex. It doesn't occur like this on most
; machines.
;--------------------------------------------------------------------------
BeginProc       MiniVDD_BroadcastHandler, DOSVM

ArgVar  dwMessage, DWORD
ArgVar  dwParam, DWORD
ArgVar  dwlParam, DWORD
ArgVar  dwRef, DWORD

                EnterProc

                cmp     ACPI_state,0
                jne     PS_Exit

                cmp     dwMessage, WM_POWERBROADCAST ;218h
                jnz     PS_Exit


                cmp     dwParam, PBT_APMRESUMESUSPEND
                jz      PS_Resume

                cmp     dwParam, PBT_APMSUSPEND
                jz              PS_Suspend

                cmp     dwParam, PBT_APMRESUMESTANDBY
                jz      PS_Resume

                cmp     dwParam, PBT_APMSTANDBY
                jz              PS_Suspend

                jmp     PS_Exit

PS_Suspend:
                
                REG_RD08(NV_PRMCIO_CRX__COLOR)  ; Save Index
                mov ah, al

                mov al, 1Fh                                             ; Unlock Extended Regs
                REG_WR08(NV_PRMCIO_CRX__COLOR)
                mov al, 57h
                REG_WR08(NV_PRMCIO_CRE__COLOR)

                ; Determine what type of display is being driven                
                mov al,28h
                REG_WR08(NV_PRMCIO_CRX__COLOR)
                REG_RD08(NV_PRMCIO_CRE__COLOR)  
                test al,80h
                jz  PS_CRT
                
                mov al,33h
                REG_WR08(NV_PRMCIO_CRX__COLOR)
                REG_RD08(NV_PRMCIO_CRE__COLOR)  
                test al,1h
                jz  PS_TV      ; it's not a DFP, it's a TV
                
                ; It's a DFP                
                push eax
                REG_RD32(NV_PRAMDAC_FP_DEBUG_0)
                or  eax,010000000h                                      ; assert PWRDOWN_FPCLK
                REG_WR32(NV_PRAMDAC_FP_DEBUG_0)
                pop  eax
                jmp skip_nvt1

PS_TV:          ; We need to turn off the TV encoder. This is problematic, so unless we're asked
                ; to do so by a customer, just treat it like a CRT.
                                

PS_CRT:                
                ; If we disable syncs, turn off DAC also
                ; a signal on RGB with syncs off may confuse the CRT
                push eax
                REG_RD32(NV_PRAMDAC_TEST_CONTROL)
                or  eax,00010000h                                       ; assert PWRDWN_DAC
                REG_WR32(NV_PRAMDAC_TEST_CONTROL)
                pop  eax
                                                
                mov al, 1Ah
                REG_WR08(NV_PRMCIO_CRX__COLOR)
                REG_RD08(NV_PRMCIO_CRE__COLOR)  ; Turn Off Vert Sync
                or al, 40h

                ; Canopus/Toshiba workaround due to ACPI patents
                ;
                ; do this always
                ;
                ;test nvCustomer, NVCUSTOMER_TOSHIBA
                ;jnz  skip_nvt1
                or al, 0c0h

skip_nvt1:

                REG_WR08(NV_PRMCIO_CRE__COLOR)

                mov al, 1Fh                                             ; Lock Extended Regs
                REG_WR08(NV_PRMCIO_CRX__COLOR)
                mov al, 0
                REG_WR08(NV_PRMCIO_CRE__COLOR)

                mov al, ah
                REG_WR08(NV_PRMCIO_CRX__COLOR)  ; Restore Index

                jmp             PS_Exit

PS_Resume:
                REG_RD08(NV_PRMCIO_CRX__COLOR)  ; Save Index
                mov ah, al

                mov al, 1Fh                                             ; Unlock Extended Regs
                REG_WR08(NV_PRMCIO_CRX__COLOR)
                mov al, 57h
                REG_WR08(NV_PRMCIO_CRE__COLOR)
                
                ; Determine what type of display is being driven                
                mov al,28h
                REG_WR08(NV_PRMCIO_CRX__COLOR)
                REG_RD08(NV_PRMCIO_CRE__COLOR)  
                test al,80h
                jz  PSR_CRT
                
                mov al,33h
                REG_WR08(NV_PRMCIO_CRX__COLOR)
                REG_RD08(NV_PRMCIO_CRE__COLOR)  
                test al,1h
                jz  PSR_TV      ; it's not a DFP, it's a TV
                
                ; It's a DFP                
                push eax
                REG_RD32(NV_PRAMDAC_FP_DEBUG_0)
                and  eax,0EFFFFFFFh                                      ; deassert PWRDOWN_FPCLK
                REG_WR32(NV_PRAMDAC_FP_DEBUG_0)
                pop  eax
                jmp skip_nvt2

PSR_TV:         ; We need to turn off (in suspend) & on the TV encoder. This is problematic, so unless we're asked
                ; to do so by a customer, just treat it like a CRT.
                                

PSR_CRT:                
                ; power the DAC up
                push eax
                REG_RD32(NV_PRAMDAC_TEST_CONTROL)
                and  eax,0FFFEFFFFh                                     ; deassert PWRDWN_DAC
                REG_WR32(NV_PRAMDAC_TEST_CONTROL)
                pop  eax

                mov al, 1Ah
                REG_WR08(NV_PRMCIO_CRX__COLOR)
                REG_RD08(NV_PRMCIO_CRE__COLOR)
                and al, not 40h

                ; Canopus/Toshiba workaround due to ACPI patents
                ;
                ; do this always
                ;
                ;test nvCustomer, NVCUSTOMER_TOSHIBA
                ;jnz skip_nvt2
                and al, not 0c0h
skip_nvt2:

                REG_WR08(NV_PRMCIO_CRE__COLOR)

                mov al, 1Fh                                             ; Lock Extended Regs
                REG_WR08(NV_PRMCIO_CRX__COLOR)
                mov al, 0
                REG_WR08(NV_PRMCIO_CRE__COLOR)

                mov al, ah
                REG_WR08(NV_PRMCIO_CRX__COLOR)  ; Restore Index

PS_Exit:

                mov             eax, 1

                LeaveProc
                Return

EndProc         MiniVDD_BroadcastHandler

;----------------------------------------
public  MiniVDD_TurnVgaOff
BeginProc       MiniVDD_TurnVgaOff, DOSVM

                push    eax
                push    esi

                ;Disable VGA through the VGA Enable IO register.
                REG_RD08(NV_PRMVIO_VSE2)
                and al,0feh
                REG_WR08(NV_PRMVIO_VSE2)

                ; This comment is only here to fix a line too long compiler error.

                ;Disable NV PCI IO, bit0 = 0.
                REG_RD32(NV_PBUS_PCI_NV_1)
                and     al,0feh
                REG_WR32(NV_PBUS_PCI_NV_1)

                ;Disable NV interrupts.
;                xor     eax,eax
;                REG_WR32(NV_PMC_INTR_EN_0)


                pop     esi
                pop     eax

                ret
EndProc         MiniVDD_TurnVgaOff
;
;----------------------------------------
public  MiniVDD_TurnVgaOn
BeginProc       MiniVDD_TurnVgaOn, DOSVM

                push    eax
                push    esi

                ;Enable NV PCI IO, bit0 = 1.
                REG_RD32(NV_PBUS_PCI_NV_1)
                or      al,01h
                REG_WR32(NV_PBUS_PCI_NV_1)

                ;Enable VGA through the VGA Enable IO register.
                REG_RD08(NV_PRMVIO_VSE2)
                or al,01h
                REG_WR08(NV_PRMVIO_VSE2)

                ;Enable NV interrupts.
                xor     eax,eax
                mov     al,1
                REG_WR32(NV_PMC_INTR_EN_0)

                pop     esi
                pop     eax

                ret
EndProc         MiniVDD_TurnVgaOn

;----------------------------------------
;Entry: none
;Exit:  ax = number of heads (crtc's)
public MiniVDD_GetNumUnits
BeginProc       MiniVDD_GetNumUnits, DOSVM
                pushad
;               Ask the RM how many heads are supported
                mov     ebx, DEFAULT_PRIMARY_HDEV
                mov     eax, NVRM_API_NUM_HEADS     ; load up the function number
                call    CallRM                      ; go to RM's API procedure
                mov     eax,ebx                     ; return in eax
                mov     NumUnits,eax                ; save in static area

;               We cannot run dual heads in VGA mode. Find out if we are in 16 color mode.
                cmp     eax,2                   ; dual CRTC device?
                jl      NumUnitsGotten          ; no, then the mode doesn't matter

;               Get Display Info from VDD
                push    ecx
                push    edi
                mov     eax,OFFSET32 DisplayInfoStructure
                mov     ecx,SIZE DISPLAYINFO    ;pass size in ECX, address in EAX
                mov     [eax].diHdrSize,cx      ;fill in this entry
                push    eax                     ;save EAX --> DisplayInfoStructure
                VxDCall VDD_Get_DISPLAYINFO     ;get information from the VDD
                pop     edi                     ;restore EAX --> DisplayInfoStructure
                mov     VGAModeFlag,0
                cmp     [edi].diBpp,4           ;16 color?
                jne     @f
                mov     VGAModeFlag,0FFh
                mov     NumUnits,1              ;Don't allow dual head if VGA.DRV is running!
@@:
                pop     edi
                pop     ecx
NumUnitsGotten:

                popad
                mov     eax,NumUnits            ;restore eax
                ret
EndProc         MiniVDD_GetNumUnits
;
VxD_ICODE_ENDS
;
;**************************************************************************************************************************
subttl          Dispatch Table for VMM Calling This Mini-VDD
page +
VxD_LOCKED_CODE_SEG
;
;
Begin_Control_Dispatch  MiniVDD
        Control_Dispatch SYS_DYNAMIC_DEVICE_INIT, MiniVDD_Dynamic_Init  ;init function for dynamic VXD
        Control_Dispatch PnP_New_DevNode, MiniVDD_PnP_New_DevNode ;init function for new pnp devnode
;        Control_Dispatch DEVICE_INIT, MiniVDD_Device_Init                              ;init function for static VXD (not us!)
        Control_Dispatch SYSTEM_EXIT, MiniVDD_System_Exit
        Control_Dispatch SYS_VM_TERMINATE, MiniVDD_Sys_VM_Terminate             ;try to release traps here
        Control_Dispatch CREATE_VM, MiniVDD_CreateVM
        Control_Dispatch DESTROY_VM, MiniVDD_DestroyVM


IFDEF DIOC
            Control_Dispatch W32_DEVICEIOCONTROL, MiniVDD_W32_DIOC
ENDIF ;DIOC

End_Control_Dispatch MiniVDD
;

PUBLIC  MiniVDD_GetMonitorInfo
BeginProc       MiniVDD_GetMonitorInfo,CCALL
ArgVar  pDevNode, DWORD
ArgVar  nUnitNum, DWORD
ArgVar  pEdidBuf, DWORD

        EnterProc
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi

        ; Stop enumeration if there is no heads anymore.
        mov     ecx,nUnitNum
        cmp     ecx,NumUnits
        mov     eax,CR_FAILURE
        jge     DoneGMI

        ; Use the logical unit number to get the physical head from
        ; the LogicalDevice table.
        lea     ebx,LogicalDevice
        shl     ecx,2
        mov     ecx,ds:[ebx + ecx]

        ; Let Windows gets EDID from the ZERO head only once.
        ; This allow Windows to show RRates list in
        ; the property page -> settings -> adapter tab.
        cmp     GetMonInfoCount,-1
        jne     @F

        ; Provide the head number for Int10Handler
        mov     GetMonInfoCount,ecx

        ; Force VESAReadEDID call
        mov     eax,CR_DEFAULT
        jmp     DoneGMI
@@:
        ; ecx = Head number
        push    pEdidBuf
        push    ecx
        push    pDevNode
        call    GetEdidAndRemapIt
        add     esp,0CH

DoneGMI:
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        LeaveProc
        Return
EndProc MiniVDD_GetMonitorInfo


PUBLIC  GetEdidAndRemapIt
BeginProc       GetEdidAndRemapIt,CCALL
ArgVar  pDevNode, DWORD
ArgVar  nPhysHeadNum, DWORD
ArgVar  pEdidBuf, DWORD

        EnterProc
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi

        mov     eax,NVRM_API_READ_EDID
        mov     ebx,DEFAULT_PRIMARY_HDEV
        mov     ecx,nPhysHeadNum
        call    CallRM

        ; Returns a ptr to the EDID in eax. If NULL, no EDID
        or      eax,eax
        je      ZeroEdid

        push    pEdidBuf
        push    eax
        call    CopyEdidOrTranslate2To1
        add     esp,8
        cmp     eax,CR_SUCCESS
        jne     ZeroEdid

        ; If the AllowMightyModes key exists, then we munge the EDID.
        ; We keep the manufacturer, etc, but change the EDID so that
        ; pretty much any mode is OK.

        push    pDevNode
        call    BuildDisplayPathXXXX
        add     esp,4

        ; Copy the path "System\CurrentControlSet....\Display\000X"
        ; into szAndMore
        lea     edx,szLocalDisplayPath
        lea     ebx,szAndMore
@@:     mov     al,[edx]
        inc     edx
        mov     [ebx],al
        inc     ebx
        or      al,al
        jne     @B

        ; Now tack on the "Display" path
        dec     ebx
        mov     byte ptr ds:[ebx + 00],'\'
        mov     byte ptr ds:[ebx + 01],'N'
        mov     byte ptr ds:[ebx + 02],'V'
        mov     byte ptr ds:[ebx + 03],'i'
        mov     byte ptr ds:[ebx + 04],'d'
        mov     byte ptr ds:[ebx + 05],'i'
        mov     byte ptr ds:[ebx + 06],'a'
        mov     byte ptr ds:[ebx + 07],'\'
        mov     byte ptr ds:[ebx + 08],'D'
        mov     byte ptr ds:[ebx + 09],'i'
        mov     byte ptr ds:[ebx + 10],'s'
        mov     byte ptr ds:[ebx + 11],'p'
        mov     byte ptr ds:[ebx + 12],'l'
        mov     byte ptr ds:[ebx + 13],'a'
        mov     byte ptr ds:[ebx + 14],'y'
        mov     byte ptr ds:[ebx + 15],0

        mov     hKey,0
        mov     esi,OFFSET szAndMore
        mov     eax,OFFSET hkey
        VMMCall _RegOpenKey, <HKEY_LOCAL_MACHINE,esi,eax>
        or      eax,eax
        mov     eax,CR_SUCCESS
        jne     DoneGER

        ; Now Read the display sub path
        mov     dwAllowMightyModes,0
        mov     dwCBType,REG_BINARY
        mov     dwCBLen,4
        mov     eax,OFFSET dwCBType
        mov     ebx,OFFSET dwAllowMightyModes
        mov     ecx,OFFSET dwCBLen
        mov     edx,OFFSET szAllowMightyModes
        VMMCall _RegQueryValueEx, <hKey, edx, 0, eax, ebx, ecx>

        ; CLose the key and zero it out.
        VMMCall _RegCloseKey, <hKey>
        mov     hKey,0

        ; So did we get back something non-0?
        mov     eax,CR_SUCCESS
        cmp     dwAllowMightyModes,0
        je      DoneGER

        ; OK, to allow all modes, we must dummy up an EDID
        push    pEdidBuf
        call    BuildMightyVersion1Edid
        add     esp,4
        mov     eax,CR_SUCCESS
        jmp     DoneGER

ZeroEdid:
        ; OK, we didn't get and EDID for this head. Unfortunately,
        ; we can't return CR_FAILURE in this case since I have
        ; found that if we return CR_FAILURE for nUnitNum=0, then
        ; we don't even get called for nUnitNum=1 (even if we are
        ; on a multihead board.) So, zero out the EDID buffer and
        ; return CR_SUCCESS.
        mov     ecx,80H / 4
        sub     eax,eax
        mov     edi,pEdidBuf
        rep     stosd
        mov     eax,CR_SUCCESS

DoneGER:
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        LeaveProc
        Return
EndProc GetEdidAndRemapIt


PUBLIC  CopyEdidOrTranslate2To1
BeginProc CopyEdidOrTranslate2To1,CCALL
ArgVar  pSrcBuf, DWORD
ArgVar  pDstBuf, DWORD

        EnterProc
        push    esi
        push    edi

        ; We got an EDID. The main VDD can only handle Version 1
        ; EDIDs, though, so if this is a version 2, then convert
        ; it to a version 1.
        mov     esi,pSrcBuf
        lea     edi,EDID1_HEADER
        mov     edx,esi
        mov     ecx,8
        repe    cmpsb
        je      DO_CopyEdid1
        cmp     byte ptr [edx],2
        je      DO_XlatEdid2

        ; Neither kind so return failure.
        mov     eax,CR_FAILURE
        jmp     Done_COT

DO_CopyEdid1:
        ; Copy EDID from SrcBuf to Dst Buf
        mov     esi,pSrcBuf
        mov     edi,pDstBuf
        mov     ecx,80H
        rep     movsb
        mov     eax,CR_SUCCESS
        jmp     Done_COT

DO_XlatEdid2:
        push    pDstBuf
        push    pSrcBuf
        call    TranslateEdid2ToEdid1
        add     esp,8
        mov     eax,CR_SUCCESS

Done_COT:
        pop     edi
        pop     esi
        LeaveProc
        Return
EndProc CopyEdidOrTranslate2To1


PUBLIC  TranslateEdid2ToEdid1
BeginProc TranslateEdid2ToEdid1,CCALL
ArgVar  pSrcBuf, DWORD
ArgVar  pDstBuf, DWORD

        EnterProc
        push    esi
        push    edi

        ; For xlatting, do minimum work needed to make VDD happy.
        ; This includes the product/vendor ID and some detailed timings.

        ; copy template
        mov     edi,pDstBuf
        mov     esi,offset32 EdidTemp
        mov     ecx,EDID1_SIZE
        rep     movsb

        ; copy vendor/product ID
        mov     esi,pSrcBuf
        mov     edi,pDstBuf
        mov     ax,[esi+EDID2_VENDOR]
        mov     [edi+EDID1_VENDOR],ax

        ; Copy detailed timings.
        ; We have to parse a bunch of fields to find them first.
        ; map of timing info tells where to start

        mov     ax,[esi+EDID2_MAP]
        mov     Edid2Map,ax
        mov     ecx,EDID2_TIMING_START

        ; any lum tables?
        test    al, EDID2_LUM_TABLE_EXIST
        jz      AEdid_rl

        ; advance beyond lum table
        mov     ah,[esi+ecx]            ; get 1st byte of timing table
        mov     al,ah
        and     al,EDID2_LUM_ENTRIES    ; get # of lum tables entries
        test    ah,EDID2_LUMX3          ; triple lum tables?
        jz      @f
        mov     bl,3
        mul     bl                      ; triple entries
@@:     add     al,1                    ; bump index beyond lum table
        add     cl,al

AEdid_rl:
        mov     ax,Edid2Map
        and     al,EDID2_RL             ; any range limits?
        jz      AEdid_drl

        ; advance beyond range limits
        shr     al,EDID2_RL_SHIFT
        mov     bl,8
        mul     bl                      ; 8-byte range limits
        add     cl,al

AEdid_drl:
        mov     ax,Edid2Map
        test    al,EDID2_DRL            ; any detailed range limits?
        jz      AEdid_tc

        ; advance beyond detailed range limits
        and     al,EDID2_DRL
        mov     bl,27
        mul     bl                      ; 27-byte detailed range limits
        add     cl,al

AEdid_tc:
        mov     ax,Edid2Map             ; get 2nd byte of map
        and     ah,EDID2_TIMING_CODES
        jz      AEdid2_dt
        shr     ah,EDID2_TC_SHIFT
        mov     al,ah
        mov     bl,4
        mul     bl                      ; 4-byte timing codes
        add     cl,al

AEdid2_dt:
        ; now we are finally at the detailed timings
        mov     ax,Edid2Map
        and     ah,EDID2_DET_TIMINGS
        mov     al,ah
        mov     bl,18
        mul     bl                      ; 18-byte detailed timings
        mov     nDetTim,al
        mov     DetTimOff,cx            ; offset within edid2 struct
                                        ;   to detailed timings

        ; copy all detailed timings to edid1
        add     edi,EDID1_DET_TIMING    ; point to edid1 detailed timings
        add     esi,ecx                 ; point to edid2 detailed timings
        xor     ecx,ecx
        mov     cl,nDetTim
        rep     movsb

        ; generate 8 bit checksum
        mov     ecx,EDID1_SIZE
        mov     edi,pEdid1
        xor     ax,ax
AEdidCs:
        add     al,[edi+ecx-1]
        loop    AEdidCs
        neg     al
        mov     [edi+EDID1_SIZE-1],al

        pop     edi
        pop     esi
        LeaveProc
        Return
EndProc TranslateEdid2ToEdid1

; We currently have a version 1 EDID.
; Change it so that it says it can do pretty much any mode.
; Let's try making one of the timing descriptors handle anything.
PUBLIC  BuildMightyVersion1Edid
BeginProc BuildMightyVersion1Edid,CCALL
ArgVar  pEdidBuf, DWORD

        EnterProc
        push    ebx
        push    esi
        push    edi

        ; ebx should point to first of 4 detailed desc
        mov     ebx,pEdidBuf
        add     ebx,36H

        ; Change a detailed timing to something big
        ; We have to parse a bunch of fields to find them first.
        ; map of timing info tells where to start

        ; In Version 1 EDIDs, the detailed timing desc / Monitor desc
        ; begins at offset 36H.  There are 4 of them and each one is
        ; 12H bytes long.

        sub     ecx,ecx
        mov     edx,0FF00H

LoopOnDesc:
        ; if the desc is of type FD (monitor range limits),
        ; then replace with our MIGHTY range limits

        ; If this is a detailed timing and not a special desc
        ; then skip it.
        mov     eax,ds:[ebx]
        and     eax,0FFFFFFH
        je      CheckDescriptor

        ; Replace the detailed timing with a detailed timing for
        ; 2048x1536@60Hz. Please note this is a GTF timing.
        mov     byte ptr ds:[ebx + 00H], 047H
        mov     byte ptr ds:[ebx + 01H], 068H
        mov     byte ptr ds:[ebx + 02H], 000H
        mov     byte ptr ds:[ebx + 03H], 0F0H
        mov     byte ptr ds:[ebx + 04H], 082H
        mov     byte ptr ds:[ebx + 05H], 000H
        mov     byte ptr ds:[ebx + 06H], 035H
        mov     byte ptr ds:[ebx + 07H], 060H
        mov     byte ptr ds:[ebx + 08H], 013H
        mov     byte ptr ds:[ebx + 09H], 0E0H
        mov     byte ptr ds:[ebx + 0AH], 013H
        mov     byte ptr ds:[ebx + 0BH], 000H
        jmp     NextDesc

CheckDescriptor:
        ; Speical desc -- but what type?
        cmp     dword ptr ds:[ebx],0FD000000H
        jne     NotFD

        mov     byte ptr ds:[ebx + 00H], 000H
        mov     byte ptr ds:[ebx + 01H], 000H
        mov     byte ptr ds:[ebx + 02H], 000H
        mov     byte ptr ds:[ebx + 03H], 0FDH
        mov     byte ptr ds:[ebx + 04H], 000H
        mov     byte ptr ds:[ebx + 05H], 03CH ; min vert in hertz
        mov     byte ptr ds:[ebx + 06H], 0C8H ; max vert in hertz
        mov     byte ptr ds:[ebx + 07H], 018H ; min horz in KHZ
        mov     byte ptr ds:[ebx + 08H], 064H ; max horz in KHZ
        ; max pix clock in MHZ / 10 so 28H = 400MHZ
        mov      byte ptr ds:[ebx + 09H], 028H
        mov      byte ptr ds:[ebx + 0AH], 000H
        mov      byte ptr ds:[ebx + 0BH], 00AH
        mov      byte ptr ds:[ebx + 0CH], 020H
        mov      byte ptr ds:[ebx + 0DH], 020H
        mov      byte ptr ds:[ebx + 0EH], 020H
        mov      byte ptr ds:[ebx + 0FH], 020H
        mov      byte ptr ds:[ebx + 10H], 020H
        mov      byte ptr ds:[ebx + 11H], 020H
        mov     dl,1

NotFD:
        cmp     dword ptr ds:[ebx],00F900000H
        je      @F
        cmp     dword ptr ds:[ebx],00FB00000H
        jne     NextDesc
@@:     mov     dh,cl

NextDesc:
        add     ebx,12H
        inc     ecx
        cmp     ecx,4
        jb      LoopOnDesc

        ; Did we replace a range desc with the new one?
        or      dl,dl
        jne     DoneFD

        ; No, we didn't. We need to replace some other desc.
        ; Did we find one that we can replace?
        cmp     dh,0FFH
        je      DoneFD

        ; dh holds the index of the desc which we can use
        sub     ebx,4 * 12H
        movzx   edx,dl
        imul    edx,12H
        add     ebx,edx
        mov     byte ptr ds:[ebx + 00H], 000H
        mov     byte ptr ds:[ebx + 01H], 000H
        mov     byte ptr ds:[ebx + 02H], 000H
        mov     byte ptr ds:[ebx + 03H], 0FDH
        mov     byte ptr ds:[ebx + 04H], 000H
        mov     byte ptr ds:[ebx + 05H], 03CH ; min vert in hertz
        mov     byte ptr ds:[ebx + 06H], 0C8H ; max vert in hertz
        mov     byte ptr ds:[ebx + 07H], 018H ; min horz in KHZ
        mov     byte ptr ds:[ebx + 08H], 064H ; max horz in KHZ
        ; max pix clock in MHZ / 10 so 28H = 400MHZ
        mov      byte ptr ds:[ebx + 09H], 028H
        mov      byte ptr ds:[ebx + 0AH], 000H
        mov      byte ptr ds:[ebx + 0BH], 00AH
        mov      byte ptr ds:[ebx + 0CH], 020H
        mov      byte ptr ds:[ebx + 0DH], 020H
        mov      byte ptr ds:[ebx + 0EH], 020H
        mov      byte ptr ds:[ebx + 0FH], 020H
        mov      byte ptr ds:[ebx + 10H], 020H
        mov      byte ptr ds:[ebx + 11H], 020H

DoneFD:
        ; Now we need to checksum the EDID.
        mov     ebx,pEdidBuf
        mov     cx,7FH
        sub     al,al

ChecksumEdid:
        add     al,byte ptr ds:[ebx]
        inc     ebx
        loop    ChecksumEdid

        neg     al
        mov     byte ptr ds:[ebx],al

        pop     edi
        pop     esi
        pop     ebx
        LeaveProc
        Return

EndProc BuildMightyVersion1Edid


PUBLIC  BuildDisplayPathXXXX
BeginProc BuildDisplayPathXXXX,CCALL
ArgVar  pDevNode, DWORD

        EnterProc
        push    ebx
        push    esi
        push    edi
        mov     ebx,pDevNode

    ; Get the correct display path in the registry
    mov     eax,[ebx + 4]
    mov     esi,OFFSET szAndMore

    ; eax now points to a string that we concatenate onto
    ; "Enum\\".  We look up this path in HKEY_LOCAL_MACHINE
@@: mov     cl,[eax]
    inc     eax
    mov     [esi],cl
    inc     esi
    or      cl,cl
    jne     @B

    ; Now open the key
    mov     hKey,0
    mov     esi,OFFSET szEnumAndMore
    mov     eax,OFFSET hkey
    VMMCall _RegOpenKey, <HKEY_LOCAL_MACHINE,esi,eax>
    or      eax,eax
    jne     UseDefaultKey

    ; Now Read the display sub path
    mov     szDisplayEndPath,0
    mov     dwCBType,REG_SZ
    mov     dwCBLen,32
    mov     eax,OFFSET dwCBType
    mov     ebx,OFFSET szDisplayEndPath
    mov     ecx,OFFSET dwCBLen
    mov     edx,OFFSET szDriver
    VMMCall _RegQueryValueEx, <hKey, edx, 0, eax, ebx, ecx>

    ; CLose the key and zero it out.
    VMMCall _RegCloseKey, <hKey>
    mov     hKey,0

    ; Now, did we get a valid string (like "DISPLAY\0005")
    cmp     szDisplayEndPath,0
    je      UseDefaultKey

    ; Make the complete path
    mov     eax,OFFSET szDisplayEndPath
    mov     ebx,OFFSET szLocalDisplayPathEnd
@@:
    mov     cl,[eax]
    inc     eax
    mov     [ebx],cl
    inc     ebx
    or      cl,cl
    jne     @B
    jmp     GotRegPath

UseDefaultKey:
    mov     eax,OFFSET szLocalDisplayPathEnd
    mov     dword ptr [eax + 0],'PSID'
    mov     dword ptr [eax + 4],'\YAL'
    mov     dword ptr [eax + 8],'0000'
    mov     byte ptr [eax + 12],0

GotRegPath:
        pop     edi
        pop     esi
        pop     ebx
        LeaveProc
        Return
EndProc BuildDisplayPathXXXX


PUBLIC  MiniVDD_CreateVM
BeginProc MiniVDD_CreateVM
        mov     eax,pfnLockOffscreen
        mov     pfnCurrentCall,eax
        call    NestedCallBack
        ret
EndProc MiniVDD_CreateVM

PUBLIC  MiniVDD_DestroyVM
BeginProc MiniVDD_DestroyVM
        mov     eax,pfnUnlockOffscreen
        mov     pfnCurrentCall,eax
        call    NestedCallBack
        ret
EndProc MiniVDD_DestroyVM


;==============================================================================
;
; Function:     DisplayDriverPowerUp
;
; Purpose:      This function call GN_PowerOn routine of display driver
;
; Arguments:    None
;
; Returns:      None
;==============================================================================
PUBLIC  DisplayDriverPowerUp
BeginProc DisplayDriverPowerUp
        mov     eax,pfnPowerOn
        mov     pfnCurrentCall,eax
        call    NestedCallBack
        ret
EndProc DisplayDriverPowerUp


;==============================================================================
;
; Function:     DisplayDriverPowerDown
;
; Purpose:      This function call GN_PowerOff routine of display driver
;
; Arguments:    None
;
; Returns:      None
;==============================================================================
PUBLIC  DisplayDriverPowerDown
BeginProc DisplayDriverPowerDown
        mov     eax,pfnPowerOff
        mov     pfnCurrentCall,eax
        call    NestedCallBack
        ret
EndProc DisplayDriverPowerDown


;==============================================================================
;
; Function:     FlashOffscreenBitmaps
;
; Purpose:      This function moves offscreen bitmaps into system memory
;
; Arguments:    None
;
; Returns:      None
;==============================================================================
PUBLIC  FlashOffscreenBitmaps
BeginProc FlashOffscreenBitmaps
        mov     eax,pfnLockOffscreen
        mov     pfnCurrentCall,eax
        call    NestedCallBack
        ret
EndProc FlashOffscreenBitmaps



; Function:     NestedCallBack
;
; Purpose:      This function simulates a far jump in a nested
;               execution block.
;
; Arguments:    None
;
; Returns:      None
;==============================================================================
PUBLIC NestedCallBack
BeginProc NestedCallBack
        pushad

        ; If we are not in the system VM, then we cannot do this!
        ; Unfortunately, then the offscreen allocations will not be
        ; flushed and there may be problems. However, I have never
        ; seen this occur. It appears that whenever we get a CreateVM
        ; or DestroyVM message and this routine gets called, that
        ; we are always in the system VM, so everything should be cool.

        VMMCall Get_Cur_VM_Handle
        push    ebx

        VMMCall Get_Sys_VM_Handle
        pop     eax

        cmp     ebx,eax
        jne     DoneNC
        cmp     dwCallbacksSelector,0
        je      DoneNC
        cmp     pfnCurrentCall,0
        je      DoneNC

        ; Save the client state and unlock all critical sections
        ; so we can get back into ring-3 code
        Push_Client_State
        VMMCall Begin_Nest_Exec

        ; Call the lock or unlock routine
        mov     ecx,dwCallbacksSelector
        mov     edx,pfnCurrentCall
        VMMCall Simulate_Far_Call
        VMMCall Resume_Exec

        ; Restore the critical section status and the execution state
        VMMCall End_Nest_Exec
        Pop_Client_State

DoneNC:
        popad
        clc
        ret

EndProc NestedCallBack


;******************************************************************************
;
;   GetBusDeviceAndFunctionNumber
;
;   DESCRIPTION:
;       Locates the device on the PCI Bus and returns the Bus, Device and Function Numbers.
;
;   ENTRY:
;       EBX = nvPhys
;       ECX = fbPhys
;       EDX = nvIRQ
;
;   EXIT:
;       EAX = EDX will now contain the needed info.
;         EDX xxxxxxxx xxxxxxxx xxxxx  xxx  xxxxxxxx
;             00000000    Bus # Device Func        IRQ # 
;
;   USES:
;       Can use all registers and flags
;
;==============================================================================
BASE0_OFFSET EQU 10h
BASE1_OFFSET EQU 14h
IRQ_OFFSET       EQU 3ch
BeginProc GetBusDeviceAndFunctionNumber

  local bus_number:BYTE
  local device_number:BYTE
  local function_number:BYTE
  local base0:DWORD
  local base1:DWORD
  local irq:BYTE
  local return_val:DWORD

  pushad

  mov   base0, ebx
  mov   base1, ecx
  mov   irq, dl
  ; Check all buses devices and functions for the memory windows and IRQ input.
  mov   bus_number,0
  .repeat
    mov   device_number,0
    .repeat
      mov   function_number,0
      .repeat
        ; Read the PCI bus and check if we have a match.
        xor   eax,eax
        mov   al, bus_number
        shl   eax,16
        mov   ah, device_number
        shl   ah,3
        or    ah, function_number
        mov   al, BASE0_OFFSET
        or    eax, 80000000h           ; eax will now contain the configuration address for the PCI read.
        ReadPCI                                ; Returns the Dword read from the PCI Config space in edx.
        and   dl,0
        .if(edx == base0)
          mov   al, BASE1_OFFSET
          ReadPCI                                  ; Returns the Dword read from the PCI Config space in edx.
          and   dl,0
          .if(edx == base1)
            mov   al, IRQ_OFFSET
            ReadPCI                                ; Returns the Dword read from the PCI Config space in edx.
            .if(dl == irq)
              ; WOW ! It works. Lets get out and return this stuff
              and   eax,00FFFF00h          ; AND off the config bit and offset.
              mov   al, irq                        ; OR in the irq
              mov   return_val, eax
              jmp   we_gots_a_match
            .endif
          .endif
        .endif
        inc   function_number
      .until function_number == 08h
      inc   device_number
    .until device_number == 01fh
    inc   bus_number
  .until bus_number == 0ffh

  mov  return_val,-1

  we_gots_a_match:
  exit_out:

  popad
  mov  edx, return_val
  ret

EndProc   GetBusDeviceAndFunctionNumber

;==============================================================================
;   DoConfigStart
;
;   DESCRIPTION:
;       Handles the CONFIG_START message.
;
;   ENTRY:
;       EBX = Devnode handle
;
;   EXIT:
;       EAX = CR_RESULT
;
;   USES:
;       Can use all registers and flags
;
;==============================================================================

BeginProc DoConfigStart

    .ERRNZ  SIZE Config_Buff_s MOD 4

    sub     esp, SIZE Config_Buff_s
    mov     edi, esp                ; alloc buffer on stack

    xor eax, eax
    cmp ConfigStartDone, 0
    jne DoConfigStartDone

    mov     dwDisplayDevnodeHandle,ebx

;   call the Configuration Manager to get the device config.
    
    VxDcall _CONFIGMG_Get_Alloc_Log_Conf, <edi, ebx, CM_GET_ALLOC_LOG_CONF_ALLOC>
    test    eax, eax
    jz      @F
    Debug_Out "MiniVDD:  Unexpected config manager error #EAX"
    jmp     DoConfigStartDone

@@:
    push        ebx
    call        BuildDisplayPathXXXX
    add         esp,4

;   We got the device config; now parse the info and call the resource manager to map it.

    ;mov    esi, [dwRefData]        ; ESI -> adapter reference data

    ;
    ; Our device should be given four or five memory windows
    ;
    ;   two for VGA (A0000 and B0000)
    ;   two for the PCI Bars (NV and FB)
    ;   one for AGP aperture/ROM (optional)
    ;
    ; We should find NV at index 2 and FB at index 3, but let's
    ; parse to make sure.
    ;
    xor             ebx,ebx
start_1:    
    ; starting with NV 4.5, we may optionally have a 32 MB fb 
    ; NV10 goes up to 128MB, so we'll check for that size too
    cmp     [edi.dMemLength][ebx*4], 08000000h      ; don't expect to find fb first, but just in case
    je      found_fb
    cmp     [edi.dMemLength][ebx*4], 02000000h      ; don't expect to find fb first, but just in case
    je      found_fb
    cmp     [edi.dMemLength][ebx*4], 01000000h
    je      @F
    inc     ebx
    cmp     bx, [edi.wNumMemWindows]
    jl      start_1
    mov     eax, CR_DEVICE_NOT_THERE
    jmp     DoConfigStartDone

@@:
    ;
    ; Found a 16meg window.  Which one is it?
    ; We should always find nv first and fb second--this is the order in the PCI config
    ; The config manager doesn't always return attrib = 5 (prefetchable) for fb, although this is set in PCI space,
    ; so we won't depend on it.
    ; We will test which window is which by reading some nv register in InitMapping (in the resource manager).
;   cmp     [edi.wMemAttrib][ebx*2], 0005h
    mov     eax, [edi.dMemBase][ebx*4]
;   je      @F
    mov     nvPhys, eax                ; nv has attrib of 1
    jmp     short start_2

; 32 or 128MB window -- must be fb
found_fb:
    mov     eax, [edi.dMemBase][ebx*4]
    mov     fbPhys, eax                ; fb has attrib of 5 (PF)
    mov     eax, [edi.dMemLength][ebx*4]
    mov     fbLength, eax
    
start_2:
    ;
    ; Find the next nv window
    ;
    inc     ebx
    cmp     bx, [edi.wNumMemWindows]    ; at end?
    jl      @F
    mov     eax, CR_DEVICE_NOT_THERE    ; fail
    jmp     DoConfigStartDone
@@:
    cmp     [edi.dMemLength][ebx*4], 08000000h      ; NV10
    je      @f
    cmp     [edi.dMemLength][ebx*4], 04000000h      ; NV20
    je      @f
    cmp     [edi.dMemLength][ebx*4], 02000000h      ; NV5
    je      @f
    cmp     [edi.dMemLength][ebx*4], 01000000h      ; NV4
    jne     start_2
    ;
    ; Found another 16meg window.  Which one is it?
    ;
@@:
; Don't count on attrib = 5! (might be fixed in Win98 final)
;    cmp     [edi.wMemAttrib][ebx*2], 0005h
    mov     eax, [edi.dMemBase][ebx*4]
;    je      @F
;       jne             @f                                                      ; yes, it must be fb
;    mov     _nvPhys, eax                ; nv has attrib of 1
;    jmp     short start_3
    cmp     fbPhys, 0
    je      @f
    mov     nvPhys, eax                 ; fb first!
    jmp     start_3
;   fb second (expected)
@@:
    mov     fbPhys, eax
    mov     eax, [edi.dMemLength][ebx*4]
    mov     fbLength, eax
        
start_3:
    ;
    ; We now have our two memory windows.  Let's look for the IRQ.
    ;
    cmp     [edi.wNumIRQs], 1
    je      @F
    mov     eax, CR_DEVICE_NOT_THERE    ; more than one irq?  ooops.
    jmp     DoConfigStartDone
@@:
    xor     eax, eax
    mov     al, [edi.bIRQRegisters][0]
    mov     nvIRQ, eax

;       check that we got something (shouldn't ever be 0 unless PCI error)
    mov     eax, CR_DEVICE_NOT_THERE    ; assume failure
    cmp     nvPhys, 0
    jz      DoConfigStartDone
    cmp     fbPhys, 0
    jz      DoConfigStartDone
    cmp     nvIRQ, 0
    jz      DoConfigStartDone

;
;   Map the physical addresses to linear addresses and install interrupt
;   Addresses should be in correct order, but we'll check in InitMapping.
map_config:

;   make a c-type call: ResManAPI(func, addr1, addr2, irq)
    push    ebx
    push    ecx
    push    edx
    push    edi
    push    esi
    mov     esi, offset32 MiniVDD_PostCRTCModeChange
    mov     edx, nvIRQ
    mov     ecx, fbPhys
    mov     ebx, nvPhys                                     ; pass physical config to RM
    ; Before we initialize in the RM, Lets pass in the Bus, Device and Function numbers along with the IRQ.
    ; This will be helpful later when we may need to turn on the device.
    ; This function will fill edx with the PCI bus, device and function info along with the IRQ.
    call    GetBusDeviceAndFunctionNumber
    mov     eax, fbLength
    shr     eax, 16
    or      ecx, eax                    ; put the upper word of the length (ie # of 64k blocks) in the lower word
    
    mov     eax, esi
    mov     esi, offset callrm_struct
    mov     dword ptr [esi].minivdd_RM_Parameter_Struct.callrm_device_handle, 1
    mov     dword ptr [esi].minivdd_RM_Parameter_Struct.callrm_modechange_callback, eax
    mov     dword ptr [esi].minivdd_RM_Parameter_Struct.callrm_irq_bus, edx
    mov     dword ptr [esi].minivdd_RM_Parameter_Struct.callrm_fbphys, ecx
    mov     dword ptr [esi].minivdd_RM_Parameter_Struct.callrm_nvphys, ebx
    lea     eax, szLocalDisplayPath
    mov     dword ptr [esi].minivdd_RM_Parameter_Struct.callrm_registry_path, eax

    mov     nvAddr, ebx                                     ; return with logical address to NV
    mov     hDev, ecx                   ; and a RM handle to the device.
    mov     fbAddr, edx                                     ; and a logical address to framebuffer
    mov     nvCustomer, edi             ; and a bitmask of customers
            
    mov     ebx, DEFAULT_PRIMARY_HDEV
    mov     eax, NVRM_API_INIT          ; load up the function number
    call    CallRM                      ; go to RM's API procedure
                                        ; do mapping to logical addresses and installing interrupt      
    mov     nvAddr, ebx                 ; return with logical address to NV
    mov     hDev, ecx                   ; and a RM handle to the device.
    mov     fbAddr, edx                 ; and a logical address to framebuffer
    mov     nvCustomer, edi             ; and a bitmask of customers
    mov     fpDDC, esi                  ; and the location of the flat panel's DDC port
    inc     numDevice                   ; track the number of times called
    pop     esi
    pop     edi
    pop     edx
    pop     ecx
    pop     ebx

    cmp     eax, RM_OK                                      
    je      @F                          ; Success.
    
    mov     eax, CR_INVALID_LOG_CONF
    jmp     DoConfigStartDone
    
@@:
    ;
    ; Everything is as it should be!!
    ;

        ; Leave comment in to handle assembler problmem

    REG_RD32(NV_PMC_BOOT_0)
    ; Decide what type of chip this is
    shr     ax, 12          ; no equates for NV4 in include yet
    cmp     ax, 4           ; is it NV4?
    jne     @f
    mov     ChipType,4      ; this won't be right for NV10, but it also doesn't look used
@@:
;
; CANOPUS TV Programming Change -- only use if customer is Canopus
;
;;   We switching off the INACTIVE bit off the "our" TV_FLAG_REGISTER in the BIOS
;;       because in VGA mode the bios should initialize (or switch off) the Chrontel
;;       It seems that if the driver does this, this will be to be to late...
;
        test nvCustomer, NVCUSTOMER_CANOPUS
        jz   PHRTV_NoCanopus01

        mov     al, bCanopusTVReg       ;-- first check, if the BIOS supports Canopus TV
        or      al, al                  
        jz      PHRTV_NoCanopus01       ; no, not supported

        call    GetCRTCPort             ;-- first unlock the registers
        mov     ax, 0571fh              ; (nobody knows what the heck the RM does...:-)
        out     dx, ax
        call    GetCRTCPort             ;-- next read TV Flag register
        mov     edi,ebx                 ;get pointer to our CB data area
        add     edi,OurCBDataPointer    ;EDI --> our CB data area for this VM
        mov     ah,[edi].PerVMData.EXT_CanopusTVReg
        test    ah, 040h                ; already initialized ???
        jnz @F                          ; yes, go on

        mov al, bCanopusTVReg           ; else get the current register contents
        out dx, al
        inc dx          
        in      al, dx
        dec dx
        mov ah, al                      ; and modify this one...
@@:
        and ah, 07fh                    ; clear the INACTIVE flag (TV_FLAG_REGISTER)
        mov al, bCanopusTVReg                   
        out dx, ax                      ; and write the new value back...
        jmp short PHRTV_NoCanopus02
        
PHRTV_NoCanopus01:
;
PHRTV_NoCanopus02:     

        ; Initialize LogicalDevice structure.
        ; LPL - pretty sure these regs aren't in use at this point,
        ; but I'm gonna push/pop 'em anyway
        push    eax
        push    ebx
        push    ecx
        push    edx

        lea     edi,LogicalDevice
        sub     ecx,ecx

@@:
IF 0
        mov     [edi],ecx       ;old way: just initialize with index
ELSE
        mov     eax, NVRM_API_DISPLAY_GET_BOOT_PHYS_MAP
        mov     ebx, DEFAULT_PRIMARY_HDEV
        push    ecx                                                     ; use counter as lookup index
        push    edi
        call    CallRM                      ; go to RM's API procedure
        pop             edi
        pop             ecx
        mov     [edi],eax                   ; return in eax
ENDIF
        inc     ecx
        add     edi,4
        cmp     ecx,MAX_CRTCS
        jb      @B

        pop             edx
        pop             ecx
        pop             ebx
        pop             eax


;We might never get the SaveMessageModeState call if running Win98, because of a bug. We should save registers here,
;because the current mode will correspond to message mode. Is this true for far east versions as well? (mode 12).
    push    ebx
    mov     ebx, WindowsVMHandle        ;save regs to system VM CB data
    call    MiniVDD_SaveRegisters
    call    MiniVDD_SaveMessageModeState ;copy to message mode VM CB data
    pop     ebx
    
    mov ConfigStartDone, 1
    mov eax, CR_SUCCESS
    
;       end up here after success or fail
DoConfigStartDone:
    add     esp, SIZE Config_Buff_s                 ; adjust stack
    cmp     eax, CR_SUCCESS
    je      DoConfigStartExit

DoConfigStartError:
    mov nvPhys, 0
    mov fbPhys, 0
    mov nvIRQ,  0
    Debug_Out "MiniVDD:  MiniVDD PnP_New_Devnode failed"
    stc
DoConfigStartExit:
    ret

EndProc DoConfigStart

;******************************************************************************
;
;   MiniVDD_PnP_New_DevNode
;
;   DESCRIPTION:
;       Handles system control calls for PnP_NEW_DEVNODE.  This routine
;       allocates the reference data for the adapter, and registers as the
;       driver for the devnode.
;
;   ENTRY:
;       EBX = Devnode handle
;       EDX = Subfunction (should always be LOAD_DRIVER)
;
;   EXIT:
;       EAX = CR_RESULT
;
;   USES:
;       Can use all registers and flags
;
;==============================================================================

BeginProc MiniVDD_PnP_New_DevNode

        cmp     edx, DLVXD_LOAD_DRIVER  ; Q: Is this the driver?
        jne     SSND_Error              ;    N: Strange!!!

;       it seems CM_Register_Device_Driver causes a problem in the Win95 VDD. The VDD won't do the VESA DDC call (4F15).
;       Avoid doing it (CM_Register_Device_Driver) in Win95.

IF 0
;   Well. It does not work on Win98 also! So Just do it the old way. The code to register for
;   config manager messages is left here commented out. Maybe someday Microsoft will fix this
;   and we can do it the documented way :-)
    VMMcall     Get_VMM_Version
    .if (!(carry?) && (ax == 040ah))
       ; This is win98. Register to get config messages.
       VxDcall _CONFIGMG_Register_Device_Driver, <ebx, OFFSET32 MiniVDD_Config_Handler, 0, CM_REGISTER_DEVICE_DRIVER_STATIC>
           .ERRNZ       CR_SUCCESS
           test eax, eax
           jnz  SSND_Couldnt_Reg_Driver
       ; Register succeeded. Clear carry and return
           clc
           jmp MiniVDD_new_devnode_Exit
    .endif
    
    ; This is win95. Can't register for config messages. So just do the config_start here.
ENDIF

    ; Just do the init here for both win95 and win98.
    call DoConfigStart
    cmp eax, CR_SUCCESS
    jne SSND_Error
    jmp MiniVDD_new_devnode_Exit
    
;   Unable to register driver.
SSND_Couldnt_Reg_Driver:
        Debug_Out "MiniVDD:  Register device driver failed for devnode #EDI"

SSND_Error:
        Debug_Out "MiniVDD:  MiniVDD PnP_New_Devnode failed"
        mov     eax, CR_FAILURE
        stc
MiniVDD_new_devnode_Exit:
        ret

EndProc MiniVDD_PnP_New_Devnode

;******************************************************************************
;
;   MiniVDD_Config_Handler
;
;   DESCRIPTION:
;       This is the plug and play driver entry point for the MiniVDD.
;       This routine is called by the config manager to notify this driver
;       of configuration related actions.
;
;   THIS PROC IS NOT USED NOW. To enable this procedure, uncomment the code
;   to register for config manager messages in MiniVDD_PnP_New_Devnode.
;
;   ENTRY:
;       cfFuncName = The function to perform.
;       sbfSubFuncName = The subfunction to perform.
;       dnToDevNode = Handle of the devnode being called.
;       dnAboutDevNode = Handle of the subject of the event.
;       ulFlags = Flags value.
;
;   EXIT:
;       EAX = CR_RESULT
;
;   USES:
;       EAX, ECX, EDX
;
;==============================================================================

BeginProc MiniVDD_Config_Handler, CCALL

ArgVar  cfFuncName, DWORD
ArgVar  scfSubFuncName, DWORD
ArgVar  dnDevNode, DWORD
ArgVar  dwRefData, DWORD
ArgVar  ulFlags, DWORD

        EnterProc
        push    ebx
        push    esi
        push    edi

        .IF ([cfFuncName] == CONFIG_FILTER)

           ;> Filter logical configurations for controller.
           ;> This is called before start and may not be required.

           mov eax, CR_SUCCESS

        .ELSEIF ([cfFuncName] == CONFIG_START)

           Debug_Out "MiniVDD:  PnP Start"
       ; We get called twice with CONFIG_START. Once with a subfunction of CONFIG_START_FIRST_START
       ; and once with a subfunction of CONFIG_START_DYNAMIC_START. We will only pay attention
       ; to CONFIG_STAR_FIRST_START.
       .IF ([scfSubFuncName] == CONFIG_START_FIRST_START)
          mov ebx, [dnDevNode]
          call DoConfigStart
       .ENDIF

        .ELSEIF ([cfFuncName] == CONFIG_STOP)

       Debug_Out "MiniVDD:  PnP Stop"
       ; need to add code to handle this.
       mov eax, CR_SUCCESS
       
        .ELSEIF ([cfFuncName] == CONFIG_TEST)

           Debug_Out "MiniVDD:  PnP Test"
       ; need to add code to handle this.
           mov  eax, CR_REMOVE_VETOED           ; if we are primary display, we can't be removed now
    
        .ELSEIF ([cfFuncName] == CONFIG_REMOVE)

       Debug_Out "MiniVDD:  PnP Remove"
           ;> The controller devnode is being removed, this driver must stop
           ;> using the allocated configuration.  Any allocated data for this
           ;> devnode should be freed.  If the driver is dynamically loaded it
           ;> will be unloaded by the config manager.  This driver will not be
           ;> called again after this call.

           ; If the user removes the device from the registry via the Device Manager,
           ; and we are the primary display, we can't just unload. We should keep displaying and the Config Manager should 
           ; take care of the registry and ask if we want to restart.
       ;        call    _rmConfigStop
           ;VMMcall _HeapFree, <dwRefData, 0>
       ;        jmp     MiniVDDPnP_Success
       ;        mov     eax, CR_REMOVE_VETOED           ; if we are primary display, we can't be removed now
       ;        jmp MiniVDDPnP_Exit
       
       ; need to add code to handle this.
           mov  eax, CR_FAILURE         ; don't allow removal

    .ELSE
       mov eax, CR_DEFAULT
    
    .ENDIF

        .ERRNZ CR_SUCCESS

; Exit with eax = status
        pop     edi
        pop     esi
        pop     ebx
        LeaveProc
        Return

EndProc MiniVDD_Config_Handler

;******************************************************************************
;
;   MiniVDD_System Exit
;
;   DESCRIPTION:        Windows is shutting down. The resource manager left the VGA
;           in an unsatisfactory state, so we should do a mode set to clean it up.
;           This is important if we are doing an exit to DOS (as opposed to a 
;           shutdown or restart).
;
;   ENTRY:
;
;   EXIT:
;       EAX = CR_RESULT
;
;   USES:
;       EAX, ECX, EDX
;
;==============================================================================

public  MiniVDD_System_Exit
BeginProc MiniVDD_System_Exit
;Entry:
;       EBX contains the VM handle of the Windows VM.
;Exit:
;
; CANOPUS TV Programming Changes -- only use if customer is Canopus
;
;;   We switching off the INACTIVE bit off the "our" TV_FLAG_REGISTER in the BIOS
;;       because in VGA mode the bios should initialize (or switch off) the Chrontel
;;       It seems that if the driver does this, this will be to be to late...
        test nvCustomer, NVCUSTOMER_CANOPUS
        jz   PHRTV_NoCanopus04
            
        push    dx
        mov     al, bCanopusTVReg       ;-- first check, if the BIOS supports Canopus TV 
        or          al, al
        jz          PHRTV_NoCanopus03   ; no, not supported

        call    GetCRTCPort             ;-- first unlock the registers
        mov     ax, 0571fh              ; (nobody knows what the heck the RM does...:-)
        out     dx, ax
        call    GetCRTCPort             ;-- next read TV Flag register
        mov         edi,ebx                     ;get pointer to our CB data area
        add         edi,OurCBDataPointer        ;EDI --> our CB data area for this VM
        mov         ah,[edi].PerVMData.EXT_CanopusTVReg
        test    ah, 040h                ; already initialized ???
        jnz     @F                      ; yes, go on

        mov     al, bCanopusTVReg       ; else get the current register contents
        out     dx, al
        inc     dx
        in          al, dx
        dec     dx
        mov     ah, al                  ; and modify this one...
@@:
        and     ah, 07fh                ; clear the INACTIVE flag (TV_FLAG_REGISTER)
        mov     al, bCanopusTVReg
        out     dx, ax                  ; and write the new value back...
PHRTV_NoCanopus03:
        pop     dx
        jmp short PHRTV_NoCanopus05
PHRTV_NoCanopus04:

;   tell the BIOS to program the TV encoder
        call    TellBIOSProgramTV
PHRTV_NoCanopus05:

        cmp             ACPI_state,0                    ; in power-down state?
        jne             @f                                              ; then don't call BIOS (chip may be disabled)

        cmp     OSVersion, VERS_MILLENNIUM  ; leave Millennium in current mode
        jge     @f

        Push_Client_State
        VMMcall Begin_Nest_V86_Exec

        ; Set mode 3 via VESA in case the Japanese language driver is running and trapping int 10.
        ; We need to do a modeset to get back to a state in which the DOS mouse driver will not try to display a cursor,
        ; and if the Japanese driver traps int 10, it won't happen, and the mouse driver will be activated and cause screen corruption.
        mov     ax,4f02h
        mov     [ebp].Client_Word_Reg_Struc.Client_AX,ax
        mov     ax,3
        mov     [ebp].Client_Word_Reg_Struc.Client_BX,ax
        ; do an int 10
        mov     eax,010h
        VMMCall Exec_Int
        
        ; set mode 3 by standard int10 to get Japanese language driver into proper state
        mov     ax,03h
        mov     [ebp].Client_Word_Reg_Struc.Client_AX,ax
        ; do an int 10
        mov     eax,010h
        VMMCall Exec_Int

        VMMCall End_Nest_Exec
        Pop_Client_State
@@:
        ret
EndProc MiniVDD_System_Exit
;

;******************************************************************************
;
;   MiniVDD_Sys_VM_Terminate
;
;   DESCRIPTION:        Windows is shutting down. We set a mode 3 to prepare for the Windows shutting down message
;
;   ENTRY:
;
;   EXIT:
;       EAX = CR_RESULT
;
;   USES:
;       EAX, ECX, EDX
;
;==============================================================================

public  MiniVDD_Sys_VM_Terminate
BeginProc MiniVDD_Sys_VM_Terminate
;We are leaving Windows and going to text mode
;Entry:
;       EBX contains the VM handle of the Windows VM.
;Exit:
        
;       Set DoNotSimulate flag to TRUE
        mov     DoNotSimulate,1         ; signal i/o trap routine that even though this is the system VM
                                        ; we should not simulate, but do I/O, so BIOS can write to DAC and CONFIG regs

        ; Unhook the Int 10 handler
        mov     eax, 10h
        mov     esi, OFFSET32 MiniVDD_Int10Handler
        VxDcall Unhook_V86_Int_Chain
        
        cmp     ACPI_state,0            ; in power-down state?
        jne     @f                      ; then don't call BIOS (chip may be disabled)

        cmp     OSVersion, VERS_MILLENNIUM  ; leave Millennium in current mode
        jge     @f

        Push_Client_State
        VMMcall Begin_Nest_V86_Exec

        ; set mode 3
        mov     ax,03h
        mov     [ebp].Client_Word_Reg_Struc.Client_AX,ax
        ; do an int 10
        mov     eax,010h
        VMMCall Exec_Int

        VMMCall End_Nest_Exec
        Pop_Client_State
@@:
        ret
EndProc MiniVDD_Sys_VM_Terminate
;
;******************************************************************************
;
;   MiniVDD_Device_Reboot_Notify
;
;   DESCRIPTION:        Windows 98 is shutting down. We set a mode 3 to prepare for the Windows shutting down message
;
;   ENTRY:
;
;   EXIT:
;       EAX = CR_RESULT
;
;   USES:
;       EAX, ECX, EDX
;
;==============================================================================

public  MiniVDD_Device_Reboot_Notify
BeginProc MiniVDD_Device_Reboot_Notify
;We are leaving Windows and going to text mode
;Entry:
;       EBX contains the VM handle of the Windows VM.
;Exit:
        
;       Set DoNotSimulate flag to TRUE
        mov     DoNotSimulate,1         ; signal i/o trap routine that even though this is the system VM
                                        ; we should not simulate, but do I/O, so BIOS can write to DAC and CONFIG regs

        cmp     ACPI_state,0            ; in power-down state?
        jne     @f                      ; then don't call BIOS (chip may be disabled)

        cmp     OSVersion, VERS_MILLENNIUM  ; leave Millennium in current mode
        jge     @f

        Push_Client_State
        VMMcall Begin_Nest_V86_Exec

        ; set mode 3
        mov     ax,03h
        mov     [ebp].Client_Word_Reg_Struc.Client_AX,ax
        ; do an int 10
        mov     eax,010h
        VMMCall Exec_Int

        VMMCall End_Nest_Exec
        Pop_Client_State
@@:
        ret
EndProc MiniVDD_Device_Reboot_Notify

subttl          Utility routines
page +
IFDEF DEBUG_LOG
public  DumpRegisters
BeginProc DumpRegisters
        pushf
        pushad
        mov     edx,MISC_INPUT
        xor     eax,eax
        in      al,dx
        Debug_Printf    "    MISC:%02lX\n\r", eax, DEBLEVELMAX
        mov     edx,FC_INPUT
        xor     eax,eax
        in      al,dx
        Debug_Printf    "    FC:%02lX\n\r", eax, DEBLEVELMAX
        mov     edx,FEAT_STATUS
        xor     eax,eax
        in      al,dx
        Debug_Printf    "    FEAT:%02lX\n\r", eax, DEBLEVELMAX
        mov     edx,INPUT_STATUS_COLOR
        xor     eax,eax
        in      al,dx
        Debug_Printf    "    INPUT_STATUS:%02lX\n\r", eax, DEBLEVELMAX
        ;
        ; DUMP Sequencer
        Debug_Printf    "    SEQ:", eax, DEBLEVELMAX
        xor     ecx,ecx
        xor     eax,eax
        mov     edx,SEQ_INDEX
        in      al,dx           ;save sequencer index
        push    eax
@@:
        mov     al,cl
        out     dx,al
        inc     edx             ;edx --> SEQ Data
        in      al,dx
        dec     edx             ;edx --> SEQ Index
        Debug_Printf    "%02lX ", eax, DEBLEVELMAX
        inc     ecx
        cmp     ecx,6
        jbe     @b
        pop     eax             ;restore sequencer index
        out     dx,al
        Debug_Printf    "\n\r", DEBLEVELMAX
        ;
        ; DUMP CRTC
        xor     ecx,ecx
        xor     eax,eax
        call    GetCRTCPort
        in      al,dx           ;save CRTC index
        push    eax
        ;
        Debug_Printf    "    CRTC:", eax, DEBLEVELMAX
@@:
        mov     al,cl
        out     dx,al
        inc     edx             ;edx --> SEQ Data
        in      al,dx
        dec     edx             ;edx --> SEQ Index
        Debug_Printf    "%02lX ", eax, DEBLEVELMAX
        inc     ecx
        cmp     ecx,00Fh
        jbe     @b
        Debug_Printf    "\n\r", DEBLEVELMAX
        ;
        Debug_Printf    "         ", eax, DEBLEVELMAX
@@:
        mov     al,cl
        out     dx,al
        inc     edx             ;edx --> SEQ Data
        in      al,dx
        dec     edx             ;edx --> SEQ Index
        Debug_Printf    "%02lX ", eax, DEBLEVELMAX
        inc     ecx
        cmp     ecx,01Fh
        jbe     @b
        Debug_Printf    "\n\r", DEBLEVELMAX
        ;
        Debug_Printf    "         ", eax, DEBLEVELMAX
@@:
        mov     al,cl
        out     dx,al
        inc     edx             ;edx --> SEQ Data
        in      al,dx
        dec     edx             ;edx --> SEQ Index
        Debug_Printf    "%02lX ", eax, DEBLEVELMAX
        inc     ecx
        cmp     ecx,02Fh
        jbe     @b
        Debug_Printf    "\n\r", DEBLEVELMAX
        ;
        Debug_Printf    "         ", eax, DEBLEVELMAX
@@:
        mov     al,cl
        out     dx,al
        inc     edx             ;edx --> SEQ Data
        in      al,dx
        dec     edx             ;edx --> SEQ Index
        Debug_Printf    "%02lX ", eax, DEBLEVELMAX
        inc     ecx
        cmp     ecx,03Fh
        jbe     @b
        Debug_Printf    "\n\r", DEBLEVELMAX
        ;
        pop     eax             ;restore CRTC index
        out     dx,al
        ;
        ; Attribute controller
        Debug_Printf    "    AC:", eax, DEBLEVELMAX
        xor     ecx,ecx
        xor     eax,eax
        call    GetCRTCPort
        in      al,dx
        push    eax             ;save CRTC index
        mov     al,024h         ;AT flip-flop register
        out     dx,al
        inc     edx             ;edx --> CRTC data
        in      al,dx
        dec     edx             ;edx --> CRTC index
        push    eax             ;save AT flip flop
        mov     al,026h         ;AT index register
        out     dx,al
        inc     edx             ;edx --> CRTC data
        in      al,dx
        dec     edx             ;edx --> CRTC index
        push    eax             ;save AT index
        ;
@@:
        mov     edx,INPUT_STATUS_COLOR
        in      al,dx           ;clear the flip-flop
        mov     edx,ATC_INDEX
        mov     al,cl
        out     dx,al
        inc     edx             ;edx --> ATC read
        in      al,dx
        dec     edx             ;edx --> ATC write
        Debug_Printf    "%02lX ", eax, DEBLEVELMAX
        inc     ecx
        cmp     ecx,014h
        jbe     @b
        ;
        mov     edx,INPUT_STATUS_COLOR
        in      al,dx           ;clear the flip-flop
        pop     eax             ;restore attribute index
        mov     edx,ATC_INDEX
        out     dx,al           ;restores index and sets flip-flop to data
        pop     eax             ;restore attribute flip-flop
        or      al,al           ;if al is zero, go back to address
        jnz     @f
        mov     edx,INPUT_STATUS_COLOR
        in      al,dx           ;clear the flip-flop
@@:
        pop     eax             ;restore CRTC index
        call    GetCRTCPort
        out     dx,al
        Debug_Printf    "\n\r", DEBLEVELMAX
        ;
        ; Graphics controller
        Debug_Printf    "    GC:", eax, DEBLEVELMAX
        xor     ecx,ecx
        xor     eax,eax
        mov     edx,GDC_INDEX
        in      al,dx           ;save sequencer index
        push    eax
@@:
        mov     al,cl
        out     dx,al
        inc     edx             ;edx --> SEQ Data
        in      al,dx
        dec     edx             ;edx --> SEQ Index
        Debug_Printf    "%02lX ", eax, DEBLEVELMAX
        inc     ecx
        cmp     ecx,8
        jbe     @b
        pop     eax             ;restore sequencer index
        out     dx,al
        Debug_Printf    "\n\r", DEBLEVELMAX
        ;
IFDEF NVIDIA_SIMULATE
        call    NVDumpSimulate
ENDIF ; NVIDIA_SIMULATE
        popad
        popf
        ret
EndProc DumpRegisters
;
;       
ENDIF ; DEBUG_LOG
;
;

public  GetCRTCPort
BeginProc GetCRTCPort
;
;       GetCRTCPort - Get the actual address of the CRTC (independent of BIOS vars)
;
;       Entry:  None
;       Exit:   DX = Address of CRTC (3D4h = color, 3B4h = mono)
;
;       Assume the VGA is awake.
;
;
        push    ax
        mov     dx,MISC_INPUT
        in      al,dx
        mov     edx,CRTC_INDEX_MONO             ; Assume mono CRTC
        test    al,1                            ; Is CRTC mono?
        jz      GetCRTCAddr_exit                ;  Yes: Go exit
        mov     edx,CRTC_INDEX_COLOR            ;  No: Return color CRTC
GetCRTCAddr_exit:
        pop     ax
        ret
EndProc GetCRTCPort
;
VxD_LOCKED_CODE_ENDS

;**************************************************************************************************************************
VxD_DATA_SEG

VDDBank                 db      ?               ;init'd at GetVDDBank
;;VDDBankControl          db      ?               ;init'd at GetVDDBank
BankRegSaveLow          db      0ffh            ;this flag must be init'd!
BankRegSaveHigh         db      0ffh            ;this flag must be init'd!
VDDPageOffsetFromBank   db      ?               ;
LatchSaveLow            db      ?               ;
LatchSaveHigh           db      ?               ;
;
LastByteInAddr                  dw              0                               ;save address of 3d0-3d3 for byte->word integration
LastByteOutAddr                 dw              0                               ;save address of 3d0-3d3 for byte->word integration
LastByteOut                             db              0                               ;save the data to output
rmState                                 db              0
rmArray                                 label   word
NvPtr                                   dd              0
NvData                                  dd              0

VxD_DATA_ENDS

;**************************************************************************************************************************
VXD_LOCKED_CODE_SEG
;
subttl          Return ChipID To Main VDD
page +
Public  MiniVDD_GetChipID
BeginProc MiniVDD_GetChipID
;
;Entry:
;       Nothing assumed.
;Exit:
;       EAX contains the ChipID.
;       Preserve ALL other registers.
;
;This routine is used by the Main VDD's Plug&Play code to determine whether
;a card has been changed since the last time that Windows was booted.  We
;are called to return to ChipID.  This assures us that Plug&Play will detect
;a different card, even if both cards use this same MiniVDD.  If the ChipID
;has changed, Plug&Play will get wind of it and will take appropriate action.
;
        push    esi
        REG_RD32(NV_PMC_BOOT_0)
        pop     esi
        ret                             ;
EndProc MiniVDD_GetChipID

public  MiniVdd_GetMonitorPowerStateCaps
BeginProc MiniVdd_GetMonitorPowerStateCaps, RARE
;
;Entry:
;       DevNode.
;Exit:
;       The CAPS of adapter
        ArgVar  DevNode, DWORD          ;message number
        EnterProc

        ; We have to support D2 because in case monitor timeout = 1 min, screensaver = 15min
        ; Windows would try to set monitor to D2 state after 1 min so we would observe
        ; screen saver if we won't support D2 (see the bug #34277).
        mov     eax, (CM_POWERSTATE_D0 or CM_POWERSTATE_D2 or CM_POWERSTATE_D3)
        LeaveProc
        Return
EndProc MiniVdd_GetMonitorPowerStateCaps

public  MiniVdd_SetMonitorPowerState
BeginProc MiniVdd_SetMonitorPowerState, RARE
;Entry:
;       DevNode.
;       PowerState.
;Exit:
;       CR_DEFAULT  - OS should POST the device
;   CR_SUCCESS  - OS should do nothing
        ArgVar  DevNode, DWORD
        ArgVar  PowerState, DWORD

        EnterProc
        push    ebx
        push    ecx
        push    edx
        
        mov     ebx,PowerState
;
;
; state n = bit n
        test    ebx,CM_POWERSTATE_D0
        jz      SMPS_D23

SMPS_D0:
                
        ; Top bit of 0 indicates that we are in a fullscreen
        ; DOS box, or in a state were DDraw shouldn't touch the HW.
        ;
        ; Top bit of 1 indicates that we are in the Windows VM. The
        ; low 31bits are individual 'enable' bits that D3D will clear
        ; as each sub-system reinits itself on returning to the Windows
        ; VM from fullscreen DOS.
        mov     eax,pDDrawFullScreenDOSActive
        or      eax,eax
        je      @F
        or      dword ptr [eax],80000000H
@@:

;       call RM to start up (use API)
        mov     eax, NVRM_API_SET_MONITOR_POWER
        mov     ebx, DEFAULT_PRIMARY_HDEV              ; device = primary
        mov     ecx,0
        call    CallRM

        jmp     SMPS_Ret

SMPS_D23:

        ; After the RM-EnableVGA, the RM has forced GET=PUT and
        ; all channels are idle. To make sure that no one renders
        ; anything or attempts to touch the HW, set some shared
        ; flags that say 'We are in a fullscreen DOS BOX -- don't
        ; do any rendering.'
        ;
        ; Top bit of 0 indicates that we are in a fullscreen
        ; DOS box. Top bit of 1 indicates that we are in
        ; the Windows VM. The low 31bits are individual 'enable'
        ; bits that D3D will clear as each sub-system reinits
        ; itself on returning to the Windows VM from fullscreen DOS.
        mov     eax,pDDrawFullScreenDOSActive
        or      eax,eax
        je      @F
        mov     dword ptr [eax],7FFFFFFFH
@@:

;       call RM to shut down (use API)
        mov     eax, NVRM_API_SET_MONITOR_POWER
        mov     ebx, DEFAULT_PRIMARY_HDEV              ; device = primary
        mov     ecx,3
        call    CallRM
        
SMPS_Ret:
        pop     edx
        pop     ecx
        pop     ebx
        mov     eax,CR_SUCCESS
        LeaveProc
        Return
EndProc MiniVdd_SetMonitorPowerState

public  MiniVdd_GetAdapterPowerStateCaps
BeginProc MiniVdd_GetAdapterPowerStateCaps, RARE
;
;Entry:
;       DevNode.
;Exit:
;       The CAPS of adapter
        ArgVar  DevNode, DWORD          ;message number
        EnterProc

;       Notify the RM that ACPI is supported on the adapter
;       And that we can expect ACPI traffic (we'll ignore APM)
        mov     ebx, DEFAULT_PRIMARY_HDEV
        mov     eax, NVRM_API_ACPI_ENABLED  ; load up the function number
        call    CallRM                      ; go to RM's API procedure

        cmp     OSVersion,VERS_MILLENNIUM
        jb      ACPI_Win98              ;Millennium or Win98?
        mov     eax, (CM_POWERSTATE_D0 or CM_POWERSTATE_D3 or CM_POWERSTATE_D1 or CM_POWERSTATE_HIBERNATE)
        jmp     GAPSC_Done
ACPI_Win98:
        mov     eax, (CM_POWERSTATE_D0 or CM_POWERSTATE_D3 or CM_POWERSTATE_D1)
GAPSC_Done:
        LeaveProc
        Return
EndProc MiniVdd_GetAdapterPowerStateCaps

subttl          Set adapter Power State
page +

public  MiniVdd_SetAdapterPowerState
BeginProc MiniVdd_SetAdapterPowerState, RARE
;Entry:
;       DevNode.
;       PowerState.
;Exit:
;       CR_DEFAULT  - OS should POST the device
;   CR_SUCCESS  - OS should do nothing
        ArgVar  DevNode, DWORD
        ArgVar  PowerState, DWORD

        EnterProc
        push    ebx
        push    ecx
        push    edx

        mov     ebx,PowerState
;       cmp     ebx,AdapterState                ; Are we already in this mode?
;       jz      SAPS_Success                    ; Yes, do nothing
;
; Check for restoring to D0, if not we must be going to D1, D2, or D3.
;
; state n = bit n
        test    ebx,CM_POWERSTATE_D3
        jnz     SAPS_D3
        test    ebx,CM_POWERSTATE_D2
        jnz     SAPS_D2
        test    ebx,CM_POWERSTATE_D1
        jnz     SAPS_D1
        test    ebx,CM_POWERSTATE_HIBERNATE
        jnz     SAPS_HIBERNATE

;   if we are going to state 3, we shut down.
;   if we are coming back to state 0, we must start up.
SAPS_D0:
        cmp     ACPI_state,0                           ; already here?
        jz      SAPS_Success
        mov     ACPI_state,0
                
;       call RM to start up (use API)
        mov     eax, NVRM_API_POWER_UP
        mov     ebx, DEFAULT_PRIMARY_HDEV              ; device = primary
        call    CallRM

        call    DisplayDriverPowerUp
        jmp     SAPS_Success

SAPS_HIBERNATE:
        call    DisplayDriverPowerDown

        mov     eax, NVRM_API_HIBERNATE
        mov     ebx, DEFAULT_PRIMARY_HDEV
        call    CallRM
        mov     ACPI_state,3

        jmp     SAPS_Success
SAPS_D3:
        cmp     ACPI_state,3        ; already here?
        jz      SAPS_Success
        mov     ACPI_state,3

        call    DisplayDriverPowerDown

;       point the real mode access at PBUS_DEBUG_1 in case the BIOS gets called to start up
        call    unlock_ext
        mov     dx,3d4h
        mov     ax,338h         ; address mode
        out     dx,ax
        mov     dx,3d0h
        mov     ax,1084h        ; lsw PBUS_DEBUG_1
        out     dx,ax
        mov     ax,0            ; msw PBUS_DEBUG_1
        mov     dx,3d2h
        out     dx,ax
        mov     dx,3d4h
        mov     ax,538h         ; data mode
        out     dx,ax
        
;       call RM to shut down (use API)
        mov     eax, NVRM_API_POWER_DOWN
        mov     ebx, DEFAULT_PRIMARY_HDEV              ; device = primary
        call    CallRM

        jmp     SAPS_Success

SAPS_D2:
;   not supported
if 0
        cmp     ACPI_state,2
        jz      SAPS_Success
        mov     ACPI_state,2
        mov     eax, NVRM_API_POWER_D2
        mov     ebx, DEFAULT_PRIMARY_HDEV              ; device = primary
        call    CallRM
        jmp     SAPS_Success
endif
        mov     eax,CR_DEFAULT
        jmp     SAPS_Ret
SAPS_D1:
        cmp     ACPI_state,1
        jz      SAPS_Success
        mov     ACPI_state,1

        call    DisplayDriverPowerDown

        mov     eax, NVRM_API_POWER_D1
        mov     ebx, DEFAULT_PRIMARY_HDEV              ; device = primary
        call    CallRM
        
SAPS_Success:
        mov     eax,CR_SUCCESS

SAPS_Ret:
        pop     edx
        pop     ecx
        pop     ebx
        LeaveProc
        Return
EndProc MiniVdd_SetAdapterPowerState

page +
public  MiniVDD_RegisterDisplayDriver
BeginProc MiniVDD_RegisterDisplayDriver, RARE
;Called from VDD's REGISTER_DISPLAY_DRIVER_INFO function
;
;Oft-times, the display driver must set flags whenever it is doing
;certain things to a hardware BLTer (such as "I'm in the middle of
;transferring a source bitmap").  This routine is called to allow
;this mini-VDD to get data (usually containing addresses inside the
;display driver's Data segment) directly from the display driver.
subttl          Register Display Driver Dependent Data

;We can also save appropriately needed states here.
;
;       The only client registers that are reserved (and cannot be used
;to pass data in) are EAX (which contains the function code which the
;display driver uses to call the "main" VDD) and EBX (which contains the
;Windows VM handle).  When we get called by the "main" VDD, we'll be
;passed a flag in Client_AL which will be set to non-zero if the VFLATD
;VxD is being used for banked support or zero if VFLATD is NOT involved in
;this session.
;
;Entry:
;       EBP --> the Client Register Structure (documented in VMM.INC)
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_RegisterDisplayDriver", DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG

        mov     InFullScreen, 0h        ; I believe, we're no longer in full screen dos mode ;-)  CANOPUS

        ; The new architecture display driver needs some functions
        ; which can only be performed at the VxD level. I made a
        ; simple interface for these services out of the minivdd
        ; function RegisterDisplayDriver. The Client_EAX register
        ; is used up because it must hold the special code
        ; VDD_REGISTER_DISPLAY_DRIVER_INFO in order to get here,
        ; and Client_EBX is used up holding the system VM handle.
        ; The other registers are all free, so:
        ; CLIENT_ecx    - sub-function
        ; All other registers are sub-function specific.
        ;
        ; I placed the sub-function constants in nvrmarch at
        ; the time I wrote this code.
        mov     eax,[ebp].Client_Reg_Struc.Client_ECX
        cmp     eax,MVDD_SET_LOCK_AND_UNLOCK_OFFSCREEN_FUNCS
        je      SetLockAndUnlockFuncs
        cmp     eax,MVDD_REENUMERATE_DEVNODE
        je      ReenumerateDevNode
        cmp     eax,MVDD_SET_POWER_MANAGEMENT_CALLBACKS
        je      SetPowerManagementCallbacks
        jmp     MRDDExit

SetLockAndUnlockFuncs:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; MVDD_SET_POWER_MANAGEMENT_CALLBACKS -- this service sets
        ; the routine that should be called when a power state is changed.
        ; Parameters:
        ; Client_ECX = MVDD_SET_LOCK_AND_UNLOCK_OFFSCREEN_FUNCS
        ; Client_EDX = selector for both the lock and unlock routine
        ; Client_ESI = offset of the lock routine (called on VM_Create)
        ; Client_EDI = offset of the unlock routine (called on VM_Destroy)
        ;
        ; Returns:      None

        mov     eax,[ebp].Client_Reg_Struc.Client_EDX
        mov     dwCallbacksSelector,eax
        mov     eax,[ebp].Client_Reg_Struc.Client_ESI
        mov     pfnLockOffscreen,eax
        mov     eax,[ebp].Client_Reg_Struc.Client_EDI
        mov     pfnUnlockOffscreen,eax
        jmp     MRDDExit

SetPowerManagementCallbacks:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; MVDD_SET_LOCK_AND_UNLOCK_OFFSCREEN_FUNCS -- this service sets
        ; the routine that should be called when a DOS box is created
        ; and destroyed.
        ; Parameters:
        ; Client_ECX = MVDD_SET_POWER_MANAGEMENT_CALLBACKS
        ; Client_EDX = selector for both the PowerOn and PowerOff routine
        ; Client_ESI = offset of the PowerOn routine (called on S0)
        ; Client_EDI = offset of the PowerOff routine (called on S1,S3,S4)
        ;
        ; Returns:      None

        mov     eax,[ebp].Client_Reg_Struc.Client_EDX
        mov     dwCallbacksSelector,eax
        mov     eax,[ebp].Client_Reg_Struc.Client_ESI
        mov     pfnPowerOn,eax
        mov     eax,[ebp].Client_Reg_Struc.Client_EDI
        mov     pfnPowerOff,eax
        jmp     MRDDExit

ReenumerateDevNode:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; MVDD_REENUMERATE_DEVNODE -- This service sets the physical
        ; head into the array LogicalDevice using the logical device
        ; as an index into the array. It then calls Windows to
        ; reenumerate the DevNode. This service is called everytime
        ; the display driver receives an Enable call.
        ; Client_ECX = MVDD_REENUMERATE_DEVNODE
        ; Client_EDX = physical head
        ; Client_ESI = logical device index

        push    ebx
        push    ecx

        mov     edx,[ebp].Client_Reg_Struc.Client_EDX   ; physical head
        mov     eax,edx                                 ; if high bit set,
        and     eax,NOT 80000000H                       ; then call reenum

        mov     ecx,[ebp].Client_Reg_Struc.Client_ESI   ; logical index
        shl     ecx,2                                   ; convert to dwords
        lea     ebx,LogicalDevice

        ; This routine realized for MAX_CRTCS == 2 case only!!!!
        mov     ds:[ebx + ecx],eax
        xor     eax,MAX_CRTCS - 1
        xor     ecx,(MAX_CRTCS-1)*4
        mov     ds:[ebx + ecx],eax

        test    edx,80000000H
        jz      @F
        VxDcall _CONFIGMG_Reenumerate_DevNode, <dwDisplayDevnodeHandle, 0>
@@:
        pop     ecx
        pop     ebx
        jmp     MRDDExit

MRDDExit:
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret                             ;
EndProc MiniVDD_RegisterDisplayDriver
;
;
subttl          Calculate and Save Banking Register Values
page +
public  MiniVDD_GetVDDBank
BeginProc MiniVDD_GetVDDBank, DOSVM
;Called from VDD's REGISTER_DISPLAY_DRIVER function
;
;In order to virtualize VGA graphics mode applications in a window on the
;Windows desktop, the VDD requires you to reserve at least 32K of off-screen
;video memory for the exclusive use of the VDD.  Although it's best to
;allocate a full 64K of off-screen video memory, the VDD can work with a
;minimum of 32K.  This function is called by the "main" VDD when the
;display driver registers with it.  This function must calculate the
;correct bank of the off-screen memory and the offset within that bank of
;the VDD reserved memory.  Note that it's best if the offset value returned
;is zero (that is, you start on a bank boundry).
;
;The mini-VDD saves the banking register values locally for later
;use during VGA virtualization.
;
;Note that the "main" VDD assumes that the hardware banks are 64K in length.
;
;Entry:
;       EBX contains the Windows VM handle (which must be preserved).
;       ECX contains the byte offset of where in video memory the VDD
;           memory is to be placed (this will be the first byte of
;           off-screen memory).
;Exit:
;       AH contains the page offset from start of bank of virtualization area.
;       EBX must still contain the Windows VM handle.
;       ECX contains the starting address of the VDD virtualization area
;       EDX contains the amount of memory that we allocated to
;       the VDD virtualization area.  We set EDX == ECX if we are in
;       a "memory shy" configuration.  That is, to tell the "main"
;       VDD that we cannot support VGA 4 plane graphics in the background.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_GetVDDBank", DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        mov     DisplayEnabledFlag,0ffh ;flag that we're in HiRes mode
        mov     SuperVGAModeFlag,0ffh   ;indicate that display driver is not VGA
        mov     BankRegSaveLow,0ffh     ;re-init this to "no banking"

;               return memory shy config
                mov             edx,0
                mov             ecx,edx

IF 0
;We won't use the following strategy. It's not worth sacrificing hi resolution 32 bit modes on a 2 Meg card just so we
;can virtualize 4 plane modes (a pretty useless exercise). The display driver will declare that we don't support 4 plane
;modes. This seems to have no effect, so we return memory-shy configuration here to tell VDD we are not supporting planar 
;emulation.
;There is also a question about the resolution of the window into the bank. The page registers seem to open a 256k window,
;so we may not be able to place the buffer on the 64k boundary which we want, effectively wasting 128k (265k - 64k instance 
; - 64k VDD buffer).

;For NV3 we are going to ignore the starting address passed by the VDD and use the space
;at the end of the framebuffer, just before the instance memory. This is because the display
;driver doesn't know how much memory is available yet, and it wants to see contiguous
;display memory, without this emulation buffer in the way.
;When we are running on a TV we do not allow planar modes, so we should never have both VDD buffer and TV filter buffer.
;               0--------------------------Framebuffer--------------------------------------- 4 Meg
;               |                       |                                                       |                               |                       |                  |
;        display buf   other bufs (direct draw)          TV bufs           VDD buf        Instance
;johnh
                pushad
                mov             eax,2                                                   ; subfunction 2
                push    eax
                VXDCall NVRM_GetAddress                                 ; return offset to VDD planar mode emulation buffer
                mov             PlanarBuf,eax
                pop             eax
                popad    
                mov             ecx,PlanarBuf                                   ; get address (already 64k aligned)
        cmp             cx,0                                                    ; well, lets make sure
        jz              @f
        xor             cx,cx                                                   ; move DOWN to 64k align
;               we are assured of 64k of aligned memory for the planar mode buffer
;               we return to the VDD the bank offset (64k) of the buffer, and save the bank number for the SetVDDBank routine.
@@:             mov             eax,ecx
                shr             eax,16                                                  ; addr/64k = bank number
                mov             VddBank,al                                              ; save bank #0
                xchg    ah,al                                                   ; return bank in ah
                mov             edx,10000h                                              ; return 64k buffer size
;               should be all there is to it
ENDIF   ; 0
                
;               this is the way to do it if we let the display driver tell us where the buffer starts (just after the display buffer).
IFDEF   OLD_BANK_STRATEGY
;
;Let's find out in which 64K bank the off-screen memory starts:
;
; MSK - TODO - DEBUG!!!!
;        mov     ecx,0001C0000h          ;Force memory available to 64KB*4planes for debugging
; MSK - TODO - DEBUG!!!!
        mov     eax,ecx                 ;get start address of off-screen memory
        shr     eax,16                  ;divide by 64K per bank -- AL has bank #
;
;At this point:
;       AL contains the bank number of the start of off-screen memory
;       CX contains the offset within the bank of the start of
;          off-screen memory.
;
;It is desirable for us to start the VDD memory area on an even bank
;boundry.  This is possible if we have at least one full 64K bank
;of video memory remaining.  To determine this, we must calculate
;the total number of banks available in video memory.  If there's
;at least one more than what's in AL, we can start on an even bank
;boundry!
;
        or      cx,cx                   ;are we already on an even bank boundry?
        jz      MGVBCalcAmountToUse     ;yes, go see if we can alloc 64K
        mov     edx,TotalMemorySize     ;get total amount of video memory
        shr     edx,16                  ;now DL has total banks on system
        dec     dl                      ;now DL has last possible bank nbr
IFNDEF ALLOW_32K_VIRTUALIZATION
        cmp     al,dl                   ;can we start on an even bank boundry?
        jae     MGVBMemoryShy           ;nope, turn off VGA virtualization
        inc     al                      ;no, put ourselves at next bank boundry
        xor     cx,cx                   ;zero the offset in low word of ECX
        add     ecx,10000h              ;and set ECX = new start of VDD area
ELSE ; ALLOW_32K_VIRTUALIZATION
        cmp     al,dl                   ;do we have any room at all?
        ja      MGVBMemoryShy           ;nope, turn off VGA virtualization
        jne     MGVBEvenBank            ;we can start on an even bank boundry.
        cmp     cx,08000h               ;do we have at least 32K left
        ja      MGVBMemoryShy           ;start above 32K, so less than 32K
        mov     cx,08000h               ;start at 32K offset
        jmp     @f
MGVBEvenBank:
        inc     al                      ;no, put ourselves at next bank boundry
        xor     cx,cx                   ;zero the offset in low word of ECX
        add     ecx,10000h              ;and set ECX = new start of VDD area
@@:
ENDIF ; ALLOW_32K_VIRTUALIZATION
;
public  MGVBCalcAmountToUse
MGVBCalcAmountToUse:
;
;At this point:
;       AL contains the bank number for VDD use.
;       ECX contains the 32 bit address of the start of this memory.
;
        mov     edx,TotalMemorySize     ;get total amount of video memory
        sub     edx,ecx                 ;EDX = amount of memory from start
                                        ;of VDD bank till end of video memory
; MSK - I added this apparently missing check for no memory left.
        jz      MGVBMemoryShy           ;do we have any memory left?
        cmp     edx,64*1024             ;do we have more than 64K left?
        mov     edx,64*1024             ;(assume we do have more than 64K left)
; MSK - I changed this to jae from ja because exactly 64K should be just fine.
; MSK - TODO - DEBUG - change to ja to debug 32K setup!!!
        jae      MGVBGetBankingValues   ;we can use an entire 64K bank for VDD!
        mov     edx,32*1024             ;we can only use 32K for the VDD!
;
public  MGVBGetBankingValues
MGVBGetBankingValues:
;
;At this point:
;       AL contains the bank number for the VDD virtualization bank.
;       ECX contains the 32 bit start address of the VDD virtualization area.
;       EDX contains the size of the VDD virtualization area (either 32K,
;           64K, or -1 indicating a memory shy configuration).
;
;We should setup the values for the NVidia banking registers so that we can set
;them quickly when called upon by the "main" VDD to do so:
;
        push    edx                     ;save size of VDD area for now
        push    ebx                     ;we need this as a work register
        mov     BankRegSaveLow,0ffh     ;make sure this wasn't messed up
                                        ;by board initialization
;        
;When the MemC is in 4 plane mode (such as when virtualizing 4 plane
;VGA apps in a window), we must actually divide the bank number that
;we calculated above (currently in AL) by 4.  If our bank number isn't
;easily divisible by 4, we must return the odd number to the main VDD
;so it can adjust accordingly:
;
        mov     ah,al                   ;copy bank number to AH for odd calc
        shr     al,2                    ;this is the physical bank that we use
        and     ah,03h                  ;this is page offset from bank's start
        shl     ah,2                    ;convert from 64K bank offset to
                                        ;number of pages (4Kx4planes=16K)
IFDEF ALLOW_32K_VIRTUALIZATION
        mov     ebx,ecx                   ;get offset into 64K bank (16Kx4planes)
        shr     ebx,14                   ;convert from bytes to pages (4Kx4planes=16K=2^14)
        add     ah,bl                   ;add to page offset
ENDIF ; ALLOW_32K_VIRTUALIZATION
        mov     VDDPageOffsetFromBank,ah
                                        ;save this for SetVDDBank
        mov     bl,al                   ;copy bank number to BL & BH for now
        shl     bl,1                    ;get bank bits in place
        mov     VDDBank,bl              ;save this for use in virtualization
        pop     ebx                     ;restore saved registers
        pop     edx                     ;
        jmp     MGVBExit                ;
;
public  MGVBMemoryShy
MGVBMemoryShy:
;
;If we reach this point, it means that the Windows visible screen overlaps
;into the very last bank of available video memory.  This creates a
;"memory-shy" configuration which prohibits us from running windowed
;and background EGA/VGA graphics mode apps.
;
;At this point:
;       ECX contains the value passed in at the beginning of the routine.
;
        mov     edx,ecx                 ;indicate a "memory-shy" configuration
;
ENDIF   ;OLD_BANK_STRATEGY

MGVBExit:
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret
EndProc MiniVDD_GetVDDBank
;
;
subttl          Banking Handling Code for VGA Virtualization
page +
public  MiniVDD_SetVDDBank
BeginProc MiniVDD_SetVDDBank, DOSVM
;
;This routine is called when we need to virtualize something in the
;off-screen region of the video memory.  You should save the current
;state of the banking registers and then set your banking registers to
;the off-screen memory region.
;
;Entry:
;       EBX contains the MemC owner's VM handle.
;Exit:
;       Save any registers that you use.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_SetVDDBank", DEBLEVELMAX
                Debug_Printf    ":MemC:%08lX", ebx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        cmp     BankRegSaveLow,0ffh     ;are we already set to the VDD bank?
        jne     MSTVExit                ;yes! don't do it again!
        cmp     SuperVGAModeFlag,0      ;running in 4 plane VGA mode?
        je      MSTVExit                ;yes, don't do anything here
        push    eax                     ;
        push    edx                     ;
;
        call    GetCRTCPort             ;EDX --> CRTC index register
        in      al,dx                   ;get current CRTC index
        ror     eax,8                   ;save it in top byte of EAX
;
;Let's make sure that the NVidia extended registers are unlocked.  This is
;probably not necessary when our CRTC VM is the Windows VM but could happen
;when the CRTC VM is not the Windows VM:
;
;        unlock_extension_regs
        call    unlock_ext

        mov     al,01Dh                 ;set to banking register
        out     dx,al                   ;
        inc     edx                     ;
        in      al,dx                   ;now AL has current bank state
        mov     BankRegSaveLow,al       ;save this for later restore
;
        mov     al,VDDBank              ;now, set the VDD bank
        out     dx,al                   ;
        dec     edx                     ;EDX --> CRTC index register
;
        mov     al,01Eh                 ;set to other banking register
        out     dx,al
        inc     edx                     ;
        in      al,dx                   ;now AL has current bank state
        mov     BankRegSaveHigh,al      ;save this for later restore
;
        mov     al,VDDBank              ;now, set the VDD bank
        out     dx,al                   ;
        dec     edx                     ;EDX --> CRTC index register
;
        rol     eax,8                   ;AL now contains saved CRTC index value
        out     dx,al                   ;

                call    restore_lock
        pop     edx                     ;restore saved registers
        pop     eax                     ;
;
MSTVExit:
        ret                             ;
EndProc MiniVDD_SetVDDBank
;
;
public  MiniVDD_ResetBank
BeginProc MiniVDD_ResetBank, DOSVM
;
;This routine is called when the VDD is done using the off-screen memory
;and we want to restore to the bank that we were in before we started using
;the off-screen memory.  Note that if the bank that's currently set in the
;hardware's banking registers is NOT the VDD bank, that we do not wish to
;reset the banking since someone else purposely changed it and we don't
;want to override those purposeful changes.
;
;Entry:
;       EBX contains the VM handle.
;Exit:
;       Save anything that you use.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_ResetBank", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        cmp     BankRegSaveLow,0ffh     ;is there any banking to restore?
        je      MRBExit                 ;nope, skip this!
;
;We may need to do something.  Save off the current CRTC index register state:
;
        push    eax                     ;
        push    edx                     ;
;
        call    GetCRTCPort             ;EDX --> CRTC index register
        in      al,dx                   ;get current CRTC index
        ror     eax,8                   ;save it in top byte of EAX
;
;Probably not necessary when our CRTC VM is the Windows VM but most likely
;is when the CRTC VM is not the Windows VM:
;
;        unlock_extension_regs
        call    unlock_ext
;
;Get the values of the banking register so we can see if its still
;set to "our" bank.  If it isn't, then Windows has already switched
;it to something for its own purposes and we'd best not restore it!
;
        mov     al,01Eh                 ;this is the NVidia banking register
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;
        dec     edx                     ;EDX --> CRTC index register
;
        cmp     al,VDDBank              ;are we still set to VDD bank?
        jne     MRBResetBankSaveFlags   ;nope, don't physically reset bank regs
;
        mov     al,01Dh                 ;this is the NVidia banking register
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;
        dec     edx                     ;EDX --> CRTC index register
;
        cmp     al,VDDBank              ;are we still set to VDD bank?
        jne     MRBResetBankSaveFlags   ;nope, don't physically reset bank regs
;
;It's safe to restore the banking registers!
;
;At this point:
;       EDX -> CRTC Data Register
;
        mov     al,01Dh                 ;this is the NVidia banking register
        mov     ah,BankRegSaveLow       ;get bank to restore
        out     dx,ax                   ;
;
        mov     al,01Eh                 ;set to other banking register
        mov     ah,BankRegSaveHigh      ;get bank to restore
        out     dx,ax                   ;
;
public  MRBResetBankSaveFlags
MRBResetBankSaveFlags:
        mov     BankRegSaveLow,0ffh     ;flag that we're not set to VDD bank
        mov     BankRegSaveHigh,0ffh     ;flag that we're not set to VDD bank
;
                call    restore_lock
;Lastly, restore the CRTC index register that we saved earlier.
        rol     eax,8                   ;AL now contains saved CRTC index value
        out     dx,al                   ;
        pop     edx                     ;restore saved registers
        pop     eax                     ;
;
MRBExit:
        ret                             ;
EndProc MiniVDD_ResetBank
;
;
subttl          Set To Latch Scratchpad Bank
page +
public  MiniVDD_SetLatchBank
BeginProc MiniVDD_SetLatchBank, DOSVM
;
;When virtualizing the VGA 4 plane mode, we have to save and restore the
;latches occasionally.  This routine allows you to set to an off-screen
;bank (in this case and in most cases, the VDD bank) in order to prepare
;for restoring the VGA latches.  This routine is NOT called for saving
;the latches since this is done by simply using the standard VGA CRTC
;register 22H which all super-VGA's possess (we hope).
;
;Entry:
;       Nothing assumed.
;Exit:
;       Save anything that you use.
;
;;IFDEF DEBUG_LOG
;;        .if (DebugOn != 0)
;;                push    eax
;;                Debug_Printf    "MiniVDD_SetLatchBank", DEBLEVELMAX
;;                pop     eax
;;        .endif
;;ENDIF ; DEBUG_LOG
        push    eax                     ;save registers that we use
        push    edx                     ;
        cmp     SuperVGAModeFlag,0      ;running in 4 plane VGA mode?
        je      MSLBExit                ;yes, don't do anything here
;
        call    GetCRTCPort             ;EDX --> CRTC index register
        in      al,dx                   ;get current CRTC index
        ror     eax,8                   ;save it in top byte of EAX
;
;Let's make sure that the NVidia extended registers are unlocked.  This is
;probably not necessary when our CRTC VM is the Windows VM but most likely
;is when the CRTC VM is not the Windows VM:
;
;        unlock_extension_regs
        call    unlock_ext
;
        mov     al,01Dh                 ;set to banking register
        out     dx,al                   ;
        inc     edx                     ;
        in      al,dx                   ;now AL has current bank state
        mov     LatchSaveLow,al         ;save this for later restore
;
        mov     al,VDDBank              ;now, set the VDD bank
        out     dx,al                   ;
        dec     edx                     ;EDX --> CRTC index register
;
        mov     al,01Eh                 ;set to other banking register
        out     dx,al
        inc     edx                     ;
        in      al,dx                   ;now AL has current bank state
        mov     LatchSaveHigh,al        ;save this for later restore
;
        mov     al,VDDBank              ;now, set the VDD bank
        out     dx,al                   ;
        dec     edx                     ;EDX --> CRTC index register
;
                call    restore_lock
        rol     eax,8                   ;AL now contains saved CRTC index value
        out     dx,al                   ;

;
MSLBExit:
        pop     edx                     ;restore saved registers
        pop     eax                     ;
        ret                             ;
EndProc MiniVDD_SetLatchBank
;
;
subttl          Reset Banking After Latch Operations
page +
public  MiniVDD_ResetLatchBank
BeginProc MiniVDD_ResetLatchBank, DOSVM
;
;This routine reverses the latch save that we did prior to restoring the
;latches.  Just restore the states that you saved.
;
;Entry:
;       Nothing assumed.
;Exit:
;       Save anything that you use.
;
;;IFDEF DEBUG_LOG
;;        .if (DebugOn != 0)
;;                push    eax
;;                Debug_Printf    "MiniVDD_ResetLatchBank", DEBLEVELMAX
;;                pop     eax
;;        .endif
;;ENDIF ; DEBUG_LOG
        push    eax                     ;save registers that we use
        push    edx                     ;
        cmp     SuperVGAModeFlag,0      ;are we running NVIDIA.DRV?
        je      MRLBExit                ;nope, don't do anything here
;
        call    GetCRTCPort             ;EDX --> CRTC index register
        in      al,dx                   ;get and save CRTC index value
        ror     eax,8                   ;

;Let's make sure that the NVidia extended registers are unlocked.  This is
;probably not necessary when our CRTC VM is the Windows VM but most likely
;is when the CRTC VM is not the Windows VM:
;
;        unlock_extension_regs
        call    unlock_ext
;
;Restore banking-enabling register:
;
        mov     al,01Dh                 ;set to new banking register
        mov     ah,LatchSaveLow         ;get bank to restore
        out     dx,ax                   ;
;
        mov     al,01Eh                 ;set to other banking register
        mov     ah,LatchSaveHigh        ;get bank to restore
        out     dx,ax

                call    restore_lock
;
;Restore the CRTC index register:
;
        rol     eax,8                   ;
        out     dx,al                   ;
;
MRLBExit:
        pop     edx                     ;restore saved registers
        pop     eax                     ;
        ret                             ;
EndProc MiniVDD_ResetLatchBank
;
;
subttl          Prepare to Enter a Standard VGA Mode from HiRes Mode
page +
public  MiniVDD_PreHiResToVGA
BeginProc MiniVDD_PreHiResToVGA, DOSVM
;
;When the VDD is about to switch from Windows HiRes mode to any of the
;standard VGA modes (for real -- not virtualized), this routine will be
;called.  You need to determine exactly what your hardware requires in
;order to accomplish this task.  For example, you should disable trapping
;on registers that the mini-VDD is specifically handling (such as 4AE8H
;in the case of the S3 chipset), make sure that the hardware is "ready"
;to undergo a mode transition (make sure that your hardware's graphics
;engine isn't in the middle of an operation that can't be interrupted)
;etc.  If your hardware does not return to a standard VGA mode via
;a call to INT 10H, function 0, you should also make sure to do whatever
;it takes to restore your hardware to a standard VGA mode at this time.
;Try not to touch video memory during your transition to standard VGA as
;this will disturb the reliability of the system.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
          push    eax
          Debug_Printf    "MiniVDD_PreHiResToVGA", DEBLEVELMAX
          pop     eax
        .endif
ENDIF ; DEBUG_LOG
        or      ModeChangeFlags,GOING_TO_VGA_MODE
        mov     DosBoxActive,1          ; DOS box active, let int 10 go thru in dual head mode

    ;
    ; CANOPUS Programming changes -- only use if customer == CANOPUS
    ;
    test nvCustomer, NVCUSTOMER_CANOPUS
    jz   dos_can1

        ; We flush OffScreen cache at this point.
        ; Our modified display driver ignores notify from CreateVM/DestroyVM.
        ; It should not disable OffScreen for windowed dos prompt.
DISABLE_OFF_SCREEN  equ 4050h
        pushad
        VMMcall Get_Cur_VM_Handle
        xchg    eax,ebx
        VMMCall Get_Sys_VM_Handle
        cmp     eax,ebx
        jne     @f

        mov     ebp,[ebx.CB_Client_Pointer]         ; EBP = Windows VM client regs ptr
        Push_Client_State                           ; allocate client regs area
        VMMcall Begin_Nest_Exec                     ; prepare to execute INT2Fh notification
        mov     [ebp.Client_AX],DISABLE_OFF_SCREEN  ; AX = high-res to VGA mode
        mov     eax,2Fh                             ; EAX = INT2Fh interrupt call
        VMMcall Exec_Int                            ; notify driver of entering VGA mode
        VMMcall End_Nest_Exec                       ;
        Pop_Client_State                            ; deallocate client regs area
@@:
        popad
dos_can1:

;
;       call RM to enable VGA (use API)

;       LPL: call below occured before flags had been set indicating
;       a switch to DOS mode, which caused race conditions (specifically,
;       when using hardware DVD players we have seen the system lock up
;       because FIFO activity continues after this disable of context switching,
;       and the system then hangs waiting for the FIFOs to empty (see bug 20000601-184547)).
;       There is a call to this same routine later in the switch to DOS.
;       We believe it's safe to remove this one.
;
;        push    ebx 
;        mov     ebx, DEFAULT_PRIMARY_HDEV
;        mov     eax, NVRM_API_ENABLEVGA
;        call    CallRM
;        pop     ebx
        
; Blank the secondary display on a dual-head adapter in case this is message mode
        push    ebx 
        mov     ebx, DEFAULT_PRIMARY_HDEV
        mov     eax, NVRM_API_BLANK_SECONDARY
        call    CallRM
        pop     ebx

        ; After the RM-EnableVGA, the RM has forced GET=PUT and
        ; all channels are idle. To make sure that no one renders
        ; anything, set some shared flags that say 'We are in a
        ; fullscreen DOS BOX -- don't do any rendering.'
        ; Top bit of 0 indicates that we are in a fullscreen
        ; DOS box. Top bit of 1 indicates that we are in
        ; the Windows VM. The low 31bits are individual 'enable'
        ; bits that D3D will clear as each sub-system reinits
        ; itself on returning to the Windows VM from fullscreen DOS.
        mov     eax,pDDrawFullScreenDOSActive
        or      eax,eax
        je      @F
        mov     dword ptr [eax],7FFFFFFFH
@@:

;
; CANOPUS TV Programming -- only use if customer is CANOPUS
;
;;   We switching off the INACTIVE bit off the "our" TV_FLAG_REGISTER in the BIOS
;;       because in VGA mode the bios should initialize (or switch off) the Chrontel
;;       It seems that if the driver does this, this will be to be to late...
;
        test    nvCustomer, NVCUSTOMER_CANOPUS
        jz      PHRTV_NoCanopus13

        mov     al, bCanopusTVReg       ;-- first check, if the BIOS supports Canopus TV 
        or          al, al                      
        jz          PHRTV_NoCanopus14   ; no, not supported

        call    GetCRTCPort             ;-- first unlock the registers
        mov     ax, 0571fh              ; (nobody knows what the heck the RM does...:-)
        out     dx, ax
        call    GetCRTCPort             ;-- next read TV Flag register
        mov         edi,ebx                     ;get pointer to our CB data area
        add         edi,OurCBDataPointer        ;EDI --> our CB data area for this VM
        mov         ah,[edi].PerVMData.EXT_CanopusTVReg
        test    ah, 040h                ; already initialized ???
        jnz     @F                      ; yes, go on

        mov     al, bCanopusTVReg       ; else get the current register contents
        out     dx, al
        inc     dx              
        in          al, dx
        dec     dx
        mov     ah, al                  ; and modify this one...
@@:
        and     ah, 07fh                ; clear the INACTIVE flag (TV_FLAG_REGISTER)
        mov     al, bCanopusTVReg                       
        out     dx, ax                  ; and write the new value back...
        jmp     short PHRTV_NoCanopus14
PHRTV_NoCanopus13:
;   tell the BIOS to program the TV encoder
        call    TellBIOSProgramTV
;
PHRTV_NoCanopus14:

;
MHTVExit:
        ret
EndProc MiniVDD_PreHiResToVGA
;
;
public  MiniVDD_PostHiResToVGA
BeginProc MiniVDD_PostHiResToVGA, DOSVM
;
;Entry:
;       EBX contains the VM handle of the new CRTC owner.
;
;This routine will be called if we're running VGA.DRV, SUPERVGA.DRV, or
;NVIDIA.DRV and applies to all drivers.
;
;This routine is called after the ROM BIOS call is made to place the hardware
;back in the standard VGA state.  You should reenable trapping and do any
;other post-processing that your hardware might need:
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_PostHiResToVGA", DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        mov     ModeChangeFlags,0       ;flag that we're done with mode change
        mov     InFullScreen, 0ffh      ; mark that we're now in full screen mode  CANOPUS

        call    unlock_ext
        call    GetCRTCPort             ; EDX --> CRTC index register

;   MOVED TO PREVGATOHIRES to allow mode switches to take effect in full screen DOS.
;   tell the BIOS NOT to program the TV encoder
;        call    GetCRTCPort
;        mov     al,BIOS_SCRATCH1          ; BIOS scratch reg
;        out     dx,al
;        inc     dx
;        in      al,dx
;        or      al,BIOS_PGRM_TV           ; set bit 1 (do not program TV encoder)
;        out     dx,al

;   Switch to VGA-compatible timing (RM modeset sets non-VGA timing for DirectX compatibility)
        mov     al,21h
        out     dx,al
        inc     dx
        in      al,dx
        and     al,0feh
        out     dx,al

        push    ebx
        add     ebx, OurCBDataPointer
        mov     [ebx].PerVMData.GoingFullscreen, 1
        pop     ebx

        ;
        ; Refresh MessageModeCBData
        ;
        ; save real mode window state
        push    eax
        push    ebx
        push    esi
        push    edx
        call    SaveNVAccessState
        mov     ebx,OFFSET32 MessageModeCBData
        mov     [ebx].PerVMData.EXT_RealModeAccess,al
        mov     [ebx].PerVMData.EXT_RealModeAddress,esi
        mov     [ebx].PerVMData.EXT_RealModeData32,dx
        ; save dac
        REG_RD32(NV_PRAMDAC_VPLL_COEFF)
        mov     [ebx].PerVMData.EXT_NV_VPLL_COEFF,eax
        REG_RD32(NV_PRAMDAC_VPLL2_COEFF)
        mov     [ebx].PerVMData.EXT_NV_VPLL2_COEFF,eax
        ; save VCLK/2
        REG_RD32(NV_PRAMDAC_PLL_COEFF_SELECT)
        mov     [ebx].PerVMData.EXT_NV_PRAMDAC,eax
        ; Save DAC General Control
        REG_RD32(NV_PRAMDAC_GENERAL_CONTROL)
        mov     [ebx].PerVMData.EXT_NV_PDAC_GEN_CTRL,eax
        ;
        pop     edx
        pop     esi
        pop     ebx
        pop     eax

;ifdef DISPLAY_SWITCH
                cmp             DisplayChanged,0                ;set in PostCRTCModeChange
                je              DispNC
                mov             DisplayChanged,0
;If display type changed, save the standard crtc regs;
;The BIOS was called to set the mode for this display, but the VDD will mess things up,
;so we'll save the CRTC's here and restore them in RestoreRegisters.
                mov             RestoreStdCRTC,1                        ; set flag for RestoreRegisters

        mov     edi,ebx                 ;get pointer to our CB data area
        add     edi,OurCBDataPointer    ;EDI --> our CB data area for this VM
                push    ebx

;;;             mov             dx,3c4h                                 ;save sequencer 1
;;;             mov             al,1
;;;             out             dx,al
;;;             inc     dx
;;;             in              al,dx
;;;             mov             [edi].PerVMData.SR1,al

                mov             dx,3d4h
                xor             ebx,ebx

;RYAN@TV: We want to save the timing regs, not the VGA regs.
;(Subtle but important difference.)  To that end, enable reading of
;the true CRTC registers
                mov             ax, 013Dh
                out             dx, ax

;       save 0-18h
savstd:
                mov             al,bl
                out             dx,al
                inc             dx
                in              al,dx
                dec             dx
                mov             [edi+ebx].PerVMData.StdCRTC,al
                inc             bx
                cmp             bx,19h
                jne             savstd

;RYAN@TV: Now, re-enable reading of the "fake" CRTC registers.
                mov             ax, 003Dh
                out             dx, ax

                pop             ebx
DispNC:
;endif ; DISPLAY_SWITCH


        call    restore_lock
;
;We have just entered a full screen VGA DOS box and none of the CRTC registers
;were trapped.  We need to get a snapshot of the NV extended registers in
;this new CRTC mode since we may have inappropriate states saved.
;
                cmp         ebx,MessageModeID   ;are we switching to Message Mode?
                je          @f                                  ;yes, don't do the save!
                call    MiniVDD_SaveRegisters   ;go save new state of registers
@@:

;IFDEF SUPER_VGA_FIX
;       may need to copy framebuffer from our save buffer
                cmp             fbBuffer,0
                jz              copyfb_done

                mov             esi,fbBuffer
                mov             edi,fbAddr
                mov             ecx,(08000h/4 - 1)
                rep     movsd
                VMMCall _HeapFree, <fbBuffer, 0>
                mov             fbBuffer,0
copyfb_done:
;ENDIF ;SUPER_VGA_FIX

MPHVExit:
        ret                             ;
EndProc MiniVDD_PostHiResToVGA
;
;
subttl          Prepare to Enter HiRes Mode From a Standard VGA Mode
page +
public  MiniVDD_PreVGAToHiRes
BeginProc MiniVDD_PreVGAToHiRes, DOSVM
;
;We are notified that the VDD is about to call the display driver to
;switch into HiRes mode from a full screen VGA DOS box.  We can do
;anything necessary to prepare for this.  In the case of the NVIDIA VDD,
;we simply set a flag telling us that we're about to change modes.
;
;Entry:
;       EBX contains the Windows VM handle.
;Exit:
;       Nothing assumed.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_PreVGAToHiRes", DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        or      ModeChangeFlags,GOING_TO_WINDOWS_MODE
        mov     DosBoxActive,0  ; back to Windows, trap int 10 in dual head mode

;
; CANOPUS TV PROGRAMMING -- only use if customer is CANOPUS
;
;;   We switching off the INACTIVE bit off the "our" TV_FLAG_REGISTER in the BIOS
;;       because in VGA mode the bios should initialize (or switch off) the Chrontel
;;       It seems that if the driver does this, this will be to be to late...
        test    nvCustomer, NVCUSTOMER_CANOPUS
        jz      PHRTV_NoCanopus23
        mov     al, bCanopusTVReg               ;-- first check, if the BIOS supports Canopus TV 
        or          al, al                      
        jz          PHRTV_NoCanopus24   ; no, not supported

        call    GetCRTCPort             ;-- first unlock the registers
        mov     ax, 0571fh              ; (nobody knows what the heck the RM does...:-)
        out     dx, ax
        call    GetCRTCPort             ;-- next read TV Flag register
        mov         edi,ebx                     ;get pointer to our CB data area
        add         edi,OurCBDataPointer        ;EDI --> our CB data area for this VM
        mov         ah,[edi].PerVMData.EXT_CanopusTVReg
        test    ah, 040h                ; already initialized ???
        jnz     @F                      ; yes, go on

        mov     al, bCanopusTVReg       ; else get the current register contents
        out     dx, al
        inc     dx              
        in          al, dx
        dec     dx
        mov     ah, al                  ; and modify this one...
@@:
        and     ah, 07fh                ; clear the INACTIVE flag (TV_FLAG_REGISTER)
        mov     al, bCanopusTVReg                       
        out     dx, ax                  ; and write the new value back...
PHRTV_NoCanopus23:
;
;   tell the BIOS NOT to program the TV encoder
        call    TellBIOSNotProgramTV
PHRTV_NoCanopus24:

        mov     InFullScreen, 0h            ; mark that we're leaving full screen mode  CANOPUS

;IFDEF SUPER_VGA_FIX
;When Super VGA modes get windowed, the VDD can't handle it. When the user returns to full screen,
;the fb content is lost. We can fix this by copying the frame buffer and restoring it when we
;return to full screen. The only problem is that the cursor position will be wrong.
                
;Find out what mode we are in
                mov             dx,3d4h
                xor             al,al                   ; read CR00
                out             dx,al
                inc             dx
                in              al,dx
                cmp             al,5fh                  ; greater than 640?
                jbe             save_not_needed ; no, VDD can handle

;In some modes, the Grabber won't correctly save and restore memory.
; We'll do it ourselves here.
                VMMCall _HeapAllocate, <8000h, 0>
                cmp             eax,0
                jz              @f

;Copy 32k
                mov             esi, fbAddr
                mov             fbBuffer,eax
                mov             edi, eax
                mov             ecx, (08000h/4 - 1)
                rep movsd
@@:
                mov             fbBuffer,eax

save_not_needed:
;ENDIF SUPER_VGA_FIX

;For NVidia, tell the resource manager we are returing to HiRes mode so it
;can restore all the contexts.

;       call RM to enable VGA (use API)
                push    ebx
                mov     ebx, DEFAULT_PRIMARY_HDEV
                mov     eax, NVRM_API_ENABLEHIRES
                call    CallRM
                pop     ebx 

;
MSVHExit:
        ret                             ;
EndProc MiniVDD_PreVGAToHiRes
;
;
public  MiniVDD_PostVGAToHiRes
BeginProc MiniVDD_PostVGAToHiRes, DOSVM
;
;We are notified that the VDD is done setting the hardware state back
;to HiRes mode.  We simply unset our ModeChangeFlags in this case.
;
;Entry:
;       EBX contains the Windows VM handle.
;Exit:
;       Nothing assumed.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_PostVGAToHiRes", DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG

        mov    ModeChangeFlags,0      ;

        ; Top bit of 0 indicates that we are in a fullscreen
        ; DOS box. Top bit of 1 indicates that we are in
        ; the Windows VM. The low 31bits are individual 'enable'
        ; bits that D3D will clear as each sub-system reinits
        ; itself on returning to the Windows VM from fullscreen DOS.
        mov     eax,pDDrawFullScreenDOSActive
        or      eax,eax
        je      @F
        or      dword ptr [eax],80000000H
@@:


;
; CANOPUS TV Programming -- only use if customer is CANOPUS
;
;;   We switching off the INACTIVE bit off the "our" TV_FLAG_REGISTER in the BIOS
;;       because in VGA mode the bios should initialize (or switch off) the Chrontel
;;       It seems that if the driver does this, this will be to be to late...
;
    test    nvCustomer, NVCUSTOMER_CANOPUS
    jz      skip_MPVTHR_Canopus01
    
        pusha

        mov     al, bCanopusTVReg       ;-- first check, if the BIOS supports Canopus TV 
        or      al, al                  
        jz      @F                      ; no, not supported

        call    GetCRTCPort             ;-- first unlock the registers
        mov     ax, 0571fh              ; (nobody knows what the heck the RM does...:-)
        out     dx, ax
        call    GetCRTCPort             ;-- next read Canopus TV register
        mov     al, bCanopusTVReg
        out     dx, al
        inc     dx              
        in      al, dx
        dec     dx
        or      al, TVF_INACTIVE        ; set the INACTIVE flag (TV_FLAG_REGISTER)
        mov     ah, al
        mov     al, bCanopusTVReg                       
        out     dx, ax                  ; and write the new value back...
@@:
        popa
skip_MPVTHR_Canopus01:
;
MPVHExit:
        ret                             ;
EndProc MiniVDD_PostVGAToHiRes
;
;
subttl          Save and Restore Routines for Extension Registers
page +
public  MiniVDD_SaveRegisters
BeginProc MiniVDD_SaveRegisters, DOSVM
;
;This routine is called whenever the "main" VDD is called upon to save
;the register state.  The "main" VDD will save all of the states of the
;"normal" VGA registers (CRTC, Sequencer, GCR, Attribute etc.) and will
;then call this routine to allow the mini-VDD to save register states
;that are hardware dependent.  These registers will be restored during the
;routine MiniVDD_RestoreRegisterState.  Typically, only a few registers
;relating to the memory access mode of the board are necessary to save
;and restore.  Specific registers that control the hardware BLTer probably
;need not be saved.  The register state is saved in the mini-VDD's
;"CB data area" which is a per-VM state saving area.  The mini-VDD's
;CB data area was allocated at Device_Init time.
;
;Entry:
;       EBX contains the VM handle for which these registers are being saved.
;Exit:
;       You must preserve EBX, EDX, EDI, and ESI.
;
; MSK - TODO - Why do other VDD's only do a few registers and why does comment
; above say only registers relating to memory access mode are needed to be saved
; and restored.
; One possible explination is that set modes are always done for switching to full
; screen dos boxes, and thus this is only needed for the MemC support for windowed
; DOS boxes.  What about message mode?  Does it do a setmode?
; idea, use protected mode interface to call the BIOS to save the state into a buffer
; Remember case of CRTC being the foreground full screen DOS box, and
; MemC being a different background DOS box.
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_SaveRegisters", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG

        cmp     nvAddr,0      ;running NVidia drivers?
        je      MSRSExit                ;nope, don't do this

        push    edx                     ;save required registers
        push    edi                     ;
        mov     edi,ebx                 ;get pointer to our CB data area
        add     edi,OurCBDataPointer    ;EDI --> our CB data area for this VM

        call    GetCRTCPort             ;EDX --> CRTC index register
        in      al,dx                   ;get and save current CRTC index ...
        ror     eax,8                   ;in high byte of EAX

        call    ReadCr44                ;Read Cr44        
        mov     [edi].PerVMData.EXT_HeadOwner,al

        .if (al == 04)
                xor     ax,ax        ;Clear ax
                call    WriteCr44    ;Set CR44 to Head A
        .endif

;
;        unlock_extension_regs
        call    unlock_ext
;
        mov     al, 19h                 ;set to CRTC index 019h
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;get data from CRTC register 019h
        dec     edx                     ;EDX --> CRTC index register
                                        ;save the data in our CB data structure
        mov     [edi].PerVMData.EXT_Repaint0NV,al
;
        mov     al, 1Ah                 ;set to CRTC index 01Ah
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;get data from CRTC register 01Ah
        dec     edx                     ;EDX --> CRTC index register
                                        ;save the data in our CB data structure
        mov     [edi].PerVMData.EXT_Repaint1NV,al
                                        ;signal that this register is valid
        mov     [edi].PerVMData.EXT_Repaint1NVSet,0FFh
;
        mov     al, 1Bh                 ;set to CRTC index 01Bh
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;get data from CRTC register 01Bh
        dec     edx                     ;EDX --> CRTC index register
                                        ;save the data in our CB data structure
        mov     [edi].PerVMData.EXT_FifoBurst,al
;
        mov     al, 1Ch                 ;set to CRTC index 01Ch
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;get data from CRTC register 01Ch
        dec     edx                     ;EDX --> CRTC index register
                                        ;save the data in our CB data structure
        mov     [edi].PerVMData.EXT_Repaint3NV,al
;
        mov     al, 1Dh                 ;set to CRTC index 01Dh
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;get data from CRTC register 01Dh
        dec     edx                     ;EDX --> CRTC index register
                                        ;save the data in our CB data structure
        mov     [edi].PerVMData.EXT_WriteBank,al
;
        mov     al, 1Eh                 ;set to CRTC index 01Eh
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;get data from CRTC register 01Eh
        dec     edx                     ;EDX --> CRTC index register
                                        ;save the data in our CB data structure
        mov     [edi].PerVMData.EXT_ReadBank,al
;
        mov     al, 20h                 ;set to CRTC index 020h
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;get data from CRTC register 020h
        dec     edx                     ;EDX --> CRTC index register
                                        ;save the data in our CB data structure
        mov     [edi].PerVMData.EXT_FifoLowWaterMark,al
;
        mov     al, 25h                 ;set to CRTC index 025h
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;get data from CRTC register 025h
        dec     edx                     ;EDX --> CRTC index register
                                        ;save the data in our CB data structure
        mov     [edi].PerVMData.EXT_ExtraBits,al
;
        mov     al, 28h                 ;set to CRTC index 028h
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;get data from CRTC register 028h
        dec     edx                     ;EDX --> CRTC index register
                                        ;save the data in our CB data structure
        mov     [edi].PerVMData.EXT_PixelFormat,al
;
        mov     al, 29h                 ;set to CRTC index 029h
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;get data from CRTC register 029h
        dec     edx                     ;EDX --> CRTC index register
                                        ;save the data in our CB data structure
        mov     [edi].PerVMData.EXT_TVDecimation,al
;
; CANOPUS TV Programming
;
        test    nvCustomer, NVCUSTOMER_CANOPUS
        jz      @f
CANOPUS_SaveRegisters:
        mov         al, bCanopusTVReg   ;set to CRTC index CanopusTVReg
        or          al, al                      ; Canopus TV supported ?
        jz          @F                  ; No, go on

        out         dx,al                       ;
        inc         edx                 ;EDX --> CRTC data register
        in          al,dx                       ;get data from Canopus TV register 
        dec         edx                 ;EDX --> CRTC index register
;; For Canopus TV we use this register as Flag register and won't like 
;; the VDD to touch bit 7
        and         al, NOT TVF_INACTIVE        ; don't touch bit 7 !!!
                                        ;save the data in our CB data structure
        mov         [edi].PerVMData.EXT_CanopusTVReg,al
@@:


        mov     al, 2Ah                 ;set to CRTC index 02Ah
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;get data from CRTC register 02Ah
        dec     edx                     ;EDX --> CRTC index register
                                        ;save the data in our CB data structure
        mov     [edi].PerVMData.EXT_OverscanHigh,al
;
        mov     al, 2Dh                 ;set to CRTC index 02Dh
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;get data from CRTC register 02Dh
        dec     edx                     ;EDX --> CRTC index register
                                        ;save the data in our CB data structure
        mov     [edi].PerVMData.EXT_HorizontalExtra,al
;
        mov     al, 30h                 ;set to CRTC index 030h
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;get data from CRTC register 030h
        dec     edx                     ;EDX --> CRTC index register
                                        ;save the data in our CB data structure
        mov     [edi].PerVMData.EXT_HWCursorAddress0,al
;
        mov     al, 31h                 ;set to CRTC index 031h
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;get data from CRTC register 031h
        dec     edx                     ;EDX --> CRTC index register
                                        ;save the data in our CB data structure
        mov     [edi].PerVMData.EXT_HWCursorAddress1,al
;
        mov     al, 32h                 ;set to CRTC index 032h
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;get data from CRTC register 032h
        dec     edx                     ;EDX --> CRTC index register
                                        ;save the data in our CB data structure
        mov     [edi].PerVMData.EXT_VideoWindow0,al
;
        mov     al, 33h                 ;set to CRTC index 033h
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;get data from CRTC register 033h
        dec     edx                     ;EDX --> CRTC index register
                                        ;save the data in our CB data structure
        mov     [edi].PerVMData.EXT_VideoWindow1,al
;
        mov     al, 39h                 ;set to CRTC index 039h
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;get data from CRTC register 039h
        dec     edx                     ;EDX --> CRTC index register
                                        ;save the data in our CB data structure
        mov     [edi].PerVMData.EXT_Interlace,al
;
; save real mode window state
        push    eax
        push    esi
        push    edx
        call    SaveNVAccessState
        mov     [edi].PerVMData.EXT_RealModeAccess,al
        mov     [edi].PerVMData.EXT_RealModeAddress,esi
        mov     [edi].PerVMData.EXT_RealModeData32,dx
IF 1
; save dac
        REG_RD32(NV_PRAMDAC_VPLL_COEFF)
        mov     [edi].PerVMData.EXT_NV_VPLL_COEFF,eax
; save VCLK/2
        REG_RD32(NV_PRAMDAC_PLL_COEFF_SELECT)
        mov     [edi].PerVMData.EXT_NV_PRAMDAC,eax
; Save DAC General Control
        REG_RD32(NV_PRAMDAC_GENERAL_CONTROL)
        mov     [edi].PerVMData.EXT_NV_PDAC_GEN_CTRL,eax
ENDIF
        ;
        pop     edx
        pop     esi
        pop     eax

                call    restore_lock

        .if ([edi].PerVMData.EXT_HeadOwner == 04)
                mov     al,04h        ;Set CR44 back to broadcast mode
                call    WriteCr44     ;Do the write thing
        .endif

;Restore the CRTC index register saved in the high byte of EAX:
;
        rol     eax,8                   ;
        out     dx,al                   ;
;
        pop     edi                     ;restore saved registers
        pop     edx                     ;
;
MSRSExit:
        ret
EndProc MiniVDD_SaveRegisters
;
;
subttl          Save and Restore Routines for Extension Registers
page +
public  MiniVDD_RestoreRegisters
BeginProc MiniVDD_RestoreRegisters, DOSVM
;
;This routine is called whenever the "main" VDD is called upon to restore
;the register state.  The "main" VDD will restore all of the states of the
;"normal" VGA registers (CRTC, Sequencer, GCR, Attribute etc.) and will
;then call this routine to allow the mini-VDD to restore register states
;that are hardware dependent.  These registers were saved during the
;routine MiniVDD_SaveRegisterState.  Typically, only a few registers
;relating to the memory access mode of the board are necessary to save
;and restore.  Specific registers that control the hardware BLTer probably
;need not be saved.  The register state is saved in the mini-VDD's
;"CB data area" which is a per-VM state saving area.  The mini-VDD's
;CB data area was allocated at Device_Init time.
;
;Entry:
;       ESI contains the VM handle for the MemC owner VM.
;       ECX contains the VM handle for the CRTC owner VM.
;Exit:
;       You must preserve EBX and EDX.  The caller preserves everything else.
;
;A short explanation of the terms "CRTC owner" and "MemC owner" is in order.
;The CRTC owner VM is the VM that owns the screen mode.  If you're running
;on the Windows desktop, then the Windows VM is the CRTC owner.  If you're
;running a full-screen DOS box, then that DOS box's VM is the CRTC owner.
;If you're running a DOS box in a window, then the CRTC owner is Windows
;but the MemC owner is the DOS VM.  What significance does this have?
;Well, when you restore the register state of a DOS VM running in a
;Window, it means that you're getting ready to VIRTUALIZE the VGA by
;using the off-screen memory.  Your VGA hardware must be setup to write
;to this memory in EXACTLY THE SAME WAY AS IF THE VGA WAS RUNNING IN
;NATIVE VGA MODE.  But...  you also must preserve the appearance of the
;Windows screen which is being displayed on the VISIBLE screen.  Thus,
;we have the screen looking like it's running in Windows HiRes packed
;pixel mode from the user's perspective, but the CPU sees the video
;memory as a 4 plane VGA.  Thus, we present this routine with both
;the CRTC owner and the MemC owner's VM handles.  Therefore, you can
;restore those states from the CRTC owner that preserve the appearance
;of the screen while restoring those states from the MemC owner that
;control how the CPU sees the video memory.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_RestoreRegisters", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                Debug_Printf    ":CRTC:%08lX", ecx, DEBLEVELMAX
                Debug_Printf    ":MemC:%08lX", esi, DEBLEVELMAX
                Debug_Printf    "\n\r", DEBLEVELMAX
                call    DumpRegisters
                pop     eax
        .endif
ENDIF ; DEBUG_LOG

        push    ebx
        push    ecx
        push    edx                     ;save caller's EDX
        push    esi


        cmp     DisplayEnabledFlag,0    ;is display in HiRes mode yet?
        je      MRRSExit                ;nope, don't do anything yet
        cmp     nvAddr,0                            ;running NVidia drivers?
        je      MRRSExit                ;nope, don't do this
;
        test    ModeChangeFlags,GOING_TO_WINDOWS_MODE OR GOING_TO_VGA_MODE
        jnz     MRRSExit                ;in a mode change, don't restore now
;
        cmp     ecx,WindowsVMHandle     ;is it the system VM?
        jz      MRRSExit                ;there are no registers which need to be restored
        
        .if (esi == MessageModeID)                      ;Does message mode own the MemC
                mov     esi,OFFSET32 MessageModeCBData  ;ESI --> Forced Message Mode CB Data
        .elseif (esi == PlanarStateID)                  ;Does planar state own the MemC
                mov     esi,OFFSET32 ForcedPlanarCBData ;ESI --> Forced Planar CB data
        .else
                add     esi,OurCBDataPointer            ;ESI --> MemC's CB data
        .endif
;
        .if (ecx == MessageModeID)                      ;Does message mode own the CRTC
                mov     ecx,OFFSET32 MessageModeCBData  ;ECX --> Forced Message Mode CB Data
        .else
                add     ecx,OurCBDataPointer            ;ECX --> CRTC's CB data

if 0   ;ifdef DISPLAY_SWITCH
; If the display device changes while a window'd DOS VM was active, we need to let the BIOS
; program the standard and extended CRTC regs, since the main VDD may have saved timings 
; for the previous device, and our saved copy of the extended CRTCs is probably stale, too.

                ;is this the VM that's about to go fullscreen?
                cmp     [ecx].PerVMData.GoingFullscreen, 1
                jne     noBiosRestore
                mov     [ecx].PerVMData.GoingFullscreen, 0

                ;is this an alphanumeric mode?
                mov             dx,     3CEh
                mov             al,     6h
                out             dx,     al
                in              ax,     dx
                and             ah,     1
                jnz             noBiosRestore

                ;did a head change occur prior to this?
                cmp             DisplayChanged,0
                jz              noBiosRestore
                mov             DisplayChanged,0

                ;let the BIOS reconfigure the CRTCs for the new active head

                ;save text cursor position
                mov             dx,     3D4h
                mov             al,     0Eh
                out             dx,     al
                in              ax,     dx
                push            eax
                inc             al
                out             dx,     al
                in              ax,     dx
                push            eax

                ;get MSL
                mov             al,     9h
                out             dx,     al
                in              ax,     dx
                movzx           bx,     ah
                and             bx,     1Fh
                inc             bx

                ;get VDE
                mov             ax,     7h
                out             dx,     al
                in              ax,     dx
                and             ax,     200h
                shl             eax,    7
                mov             al,     12h
                out             dx,     al
                in              ax,     dx
                shr             eax,    8
                inc             ax

                ;compute VDE/MSL
                xor             dx,     dx
                idiv            bx
                mov             cx,     ax      ;number of text rows on the screen

                ;set the mode 
                mov             ax,     0083h   ;nondestructive mode 3
                push            10h
                VMMCall         Exec_VxD_Int
                mov             ax,     1201h   ;350 scanlines
                cmp             cx,     43
                je              @F
                mov             ax,     1202h   ;400 scanlines
@@:             mov             bl,     30h
                push            10h
                VMMCall         Exec_VxD_Int
                mov             ax,     0083h   ;nondestructive mode 3
                push            10h
                VMMCall         Exec_VxD_Int
                mov             ax,     1112h   ;load 8x8 font (43/50 lines)
                cmp             cx,     25
                jne             @F
                mov             ax,     1114h   ;load 8x16 font (25 lines)
@@:             mov             bl,     0
                push            10h             ;the BIOS infers dot clock and sync
                VMMCall         Exec_VxD_Int    ;polarities from this and the above

                ;restore cursor position
                mov             dx,     3D4h
                pop             eax
                out             dx,     ax
                pop             eax
                out             dx,     ax

                jmp             MRRSExit

noBiosRestore:
endif ;endif ; DISPLAY_SWITCH

        .endif
;
MRRSGotAddr:

        call    GetCRTCPort             ;EDX --> CRTC index register
        in      al,dx                   ;get and save current CRTC index ...
        ror     eax,8                   ;in high byte of EAX

; Broadcast mode support
; If we intend to support broadcast mode, we need to save either
; both heads or one head data here.  Either way, we need to turn off
; broadcast mode if it's set so we don't override
; Here's the current decision tree
; save current_mode on stack
; if (current_mode == broadcast)
;     restore previous head only and return to broadcast mode
; else
;     restore without changing head at all
;
; Old code was not broadcast mode aware.  It just assumed that CR44
; pointed to the active head.  So we don't want to change that.
;
        call    ReadCr44        ; GetCr44 in al
        
        .if (al == 04)
                ; currently we're in broadcast
                or      al,80h                           ;flag that we need to restore CR44
                ror     eax,8                            ;store al in highest byte
                                                         ;original index now in next highest byte
                mov     al,[ecx].PerVMData.EXT_HeadOwner ;retrieve stored CR44
                call    WriteCr44
        .else
                ror     eax,8                            ;not needed, but for consistency
                                                         ;original index now in next highest byte
        .endif

;        unlock_extension_regs
        call    unlock_ext    
;
;We need to restore the state of the saved CRTC and Sequencer registers:
;
; MSK - NOTE: SetVDDBank does not change banking control in 01Ah
; it depends on things being write after RESTORE_REGISTERS because
; that is what the Windows 95 DDK documentation under GET_VDD_BANK
; says to do.
        mov     al,19h                  ;set to CRTC index 19h
                                        ;get CRTC owner's value for this reg
        mov     ah,[ecx].PerVMData.EXT_Repaint0NV
        out     dx,ax                   ;
;
        mov     al,1Ah                  ;set to CRTC index 1Ah
        mov     ah,[ecx].PerVMData.EXT_Repaint1NV ;get CRTC owner's value for this reg
        .if (ecx == OFFSET32 MessageModeCBData)   ;Does message mode own the CRTC?
                and     ah,3fh                    ;we can enable syncs; we may have saved wrong setting if we loaded on another device
        .else
                in al,dx                ; get current value
                and al,0c0h
                and ah,3fh
                or  ah,al               ; use current sync settings
                mov al,1Ah
        .endif
        out     dx,ax                   ;
;
        mov     al,1Bh                  ;set to CRTC index 1Bh
                                        ;get CRTC owner's value for this reg
        mov     ah,[ecx].PerVMData.EXT_FifoBurst
        out     dx,ax                   ;
;
        ; We have to be a little careful with index 1C, since it holds the
        ; FIFO Burst Mode (bit 5), which indicates how the value in the FIFO
        ; Burst register (1B) should be interpreted (NV11 arbitration sets it
        ; to 0x1). Take everything, but bit 5, from MemC owner's (esi) ptr.

        push    ebx
        mov     al,1Ch                  ;set to CRTC index 1Ch
                                        ;get MemC owner's value for this reg
        mov     ah,[esi].PerVMData.EXT_Repaint3NV
        and     ah,NOT 20h              ;clear Fifo Burst Mode Bit
        mov     bh,[ecx].PerVMData.EXT_Repaint3NV
        and     bh,20h                  ;get CRTC owner's value for Fifo Burst Mode
        or      ah,bh                   ;combine the values
        out     dx,ax                   ;
        pop     ebx        
;
        cmp     esi,ecx                 ;Only restore banking if MemC owns the CRTC
                                        ;(also try using CRTC, but only if not the Windows VM)
        jne     @f
        mov     al,1Dh                  ;set to CRTC index 1Dh
                                        ;get MemC owner's value for this reg
        mov     ah,[esi].PerVMData.EXT_WriteBank
        out     dx,ax                   ;
;
        mov     al,1Eh                  ;set to CRTC index 1Eh
                                        ;get MemC owner's value for this reg
        mov     ah,[esi].PerVMData.EXT_ReadBank
        out     dx,ax                   ;
@@:
;
        mov     al,20h                  ;set to CRTC index 20h
                                        ;get CRTC owner's value for this reg
        mov     ah,[ecx].PerVMData.EXT_FifoLowWaterMark
        out     dx,ax                   ;
;
        mov     al,25h                  ;set to CRTC index 25h
                                        ;get CRTC owner's value for this reg
        mov     ah,[ecx].PerVMData.EXT_ExtraBits
        out     dx,ax                   ;
;
        mov     al,28h                  ;set to CRTC index 28h
                                        ;get CRTC owner's value for this reg
        mov     ah,[ecx].PerVMData.EXT_PixelFormat
        out     dx,ax                   ;
;
        mov     al,29h                  ;set to CRTC index 29h
                                        ;get CRTC owner's value for this reg
        mov     ah,[ecx].PerVMData.EXT_TVDecimation
        out     dx,ax                   ;

        ; CANOPUS TV Programming
        ;
        test    nvCustomer, NVCUSTOMER_CANOPUS
        jz      @f
                ;; for Canopus TV we use this register as Flag register and won't like 
                ;;       the VDD to touch bit 7 !!!
CANOPUS_RestoreRegisters:
        mov         al, bCanopusTVReg   ;set to CRTC index Canopus TV register
        or          al, al                          ; Canopus TV supported ?
        jz          @F                          ; No, go on

        out         dx, al
        inc         dx
        in          al, dx                          ; get the current value of the register
        dec         dx
        and         al, TVF_INACTIVE    ; mask out the bit we won't touch..
        mov         ah, al                          ; move value to high byte
                                                    ;get CRTC owner's value for this reg
        or          ah,[ecx].PerVMData.EXT_CanopusTVReg
        mov         al, bCanopusTVReg   ;set to canopus Tv register
        out         dx,ax                           ;
@@:
        
;
        mov     al,2Ah                  ;set to CRTC index 2Ah
                                        ;get CRTC owner's value for this reg
        mov     ah,[ecx].PerVMData.EXT_OverscanHigh
        out     dx,ax                   ;
;
        mov     al,2Dh                  ;set to CRTC index 2Dh
                                        ;get CRTC owner's value for this reg
        mov     ah,[ecx].PerVMData.EXT_HorizontalExtra
        out     dx,ax                   ;
;
; JJV - Don't bother restoring the HW cursor address.
;;;;;        mov     al,30h                  ;set to CRTC index 30h
;;;;;                                        ;get CRTC owner's value for this reg
;;;;;        mov     ah,[ecx].PerVMData.EXT_HWCursorAddress0
;;;;;        out     dx,ax                   ;
;;;;;;
;;;;;        mov     al,31h                  ;set to CRTC index 31h
;;;;;                                        ;get CRTC owner's value for this reg
;;;;;        mov     ah,[ecx].PerVMData.EXT_HWCursorAddress1
;;;;;        out     dx,ax                   ;
        mov     al,32h                  ;set to CRTC index 32h
                                        ;get CRTC owner's value for this reg
        mov     ah,[ecx].PerVMData.EXT_VideoWindow0
        out     dx,ax

; read current CRTC to get device bits
        mov     al,33h                  ;set to CRTC index 33h
        out     dx,al                   ;get current value for this reg
        inc     dx
        in      al,dx
        and     al,11h                  ;get slave bit
        dec     dx
        mov     ah,[ecx].PerVMData.EXT_VideoWindow1 ;get CRTC owner's value for this reg
        and     ah,0eeh                 ;mask slave bit and FP data bit
        or      ah,al                   ;add slave bit
        mov     al,33h
        out     dx,ax                   ;
;
        mov     al,39h                  ;set to CRTC index 39h
                                        ;get CRTC owner's value for this reg
        mov     ah,[ecx].PerVMData.EXT_Interlace
        out     dx,ax                   ;
;
        ; Restore DAC clock
        push    eax
        push    esi
        push    edx
        ;
        mov     eax,[ecx].PerVMData.EXT_NV_VPLL_COEFF
        REG_WR32(NV_PRAMDAC_VPLL_COEFF)

;
        ; Restore VCLK/2
        mov     eax,[ecx].PerVMData.EXT_NV_PRAMDAC
        REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT)

; Restore DAC General Control
        mov     eax,[ecx].PerVMData.EXT_NV_PDAC_GEN_CTRL
;comment
        REG_WR32(NV_PRAMDAC_GENERAL_CONTROL)
        ;
        ; restore real mode window state
        mov     al,[ecx].PerVMData.EXT_RealModeAccess
        mov     esi,[ecx].PerVMData.EXT_RealModeAddress
        mov     dx,[ecx].PerVMData.EXT_RealModeData32
        call    RestoreNVAccessState

;ifdef DISPLAY_SWITCH
; If we allow the display device to change while a DOS session is active, we also need to
; restore the standard CRTC regs, since the main VDD may have saved timings for the previous
; device. We save std crtc's in postHiResToVga in this case.
                cmp             RestoreStdCrtc,0
                jz              noStdRestore

                mov             RestoreStdCrtc,0
                push    ebx
                mov             dx,3d4h
                xor             ebx,ebx

                mov             al,21h          ;unshadow
                out             dx,al
                inc             dx
                in              al,dx
                dec             dx
                shl             eax,16          ;save shadow bits in eax upper word
                mov             ax,0ff21h       ;enable all
                out             dx,ax
                
                mov             al,11h          ;write enable 0-7
                out             dx,al
                inc             dx
                in              al,dx
                and             al,07fh
                out             dx,al
                dec             dx                              
stdlp:
                mov             al,bl
                mov             ah,[ecx+ebx].PerVMData.StdCRTC
                out             dx,ax           
                inc             bx
                cmp             bx,0eh          ;skip e and f
                jnz             @f
                add             bx,2
@@:             cmp             bx,19h
                jne             stdlp           
                
                pop             ebx
                shr             eax,8           ;restore shadow bits
                mov             al,21h
                out             dx,ax           

                mov             al,11h          ;restore 0-7 write protect
                out             dx,al
                inc             dx
                in              al,dx
                or              al,80h
                out             dx,al

;;;             mov             ah,[ecx].PerVMData.SR1  ; restore SR1
;;;             mov             dx,3c4h
;;;             mov             al,1
;;;             out             dx,ax
noStdRestore:
;endif ; DISPLAY_SWITCH

        ;
        pop     edx
        pop     esi
        pop     eax

                call    restore_lock
                
        rol     eax,8   ;restore eax to determine if CR44 needs resetting
        
        ; Restore Head owner if needed
        .if (al & 80h)            ;if saved_mode needs to be restored
                and     al,7fh    ;remove flag
                call    WriteCr44 ;Write it back
        .endif

;
;Restore the CRTC index register saved in the high byte of EAX:
;
        rol     eax,8                   ;
        out     dx,al                   ;
;
MRRSExit:
        pop     esi
        pop     edx
        pop     ecx                     ;
        pop     ebx
        ret                             ;
EndProc MiniVDD_RestoreRegisters
;
;
subttl          Save and Restore Latches
page +
public  MiniVDD_SaveLatches
BeginProc MiniVDD_SaveLatches, DOSVM
;
;When virtualizing the VGA 4 plane mode, we have to save and restore the
;latches occasionally.  This routine is called for saving
;the latches by simply using the standard VGA CRTC
;register 22H which all super-VGA's possess (we hope).
;
;Entry:
;       EBX contains a Windows VM handle (which must be preserved).
;       EBP --> VM's Client Registers.
;Exit:
;       Save anything that you use.
;
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_SaveLatches", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX\n\r", ebx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        push    eax
        push    ebx
        push    ecx
        push    edx
        add     ebx,OurCBDataPointer    ;EBX --> VM's CB data area


        mov     edx,GDC_INDEX           ;EDX --> GC index register
        in      al,dx
        ror     eax,8                   ;save index

        mov     al,004h
        out     dx,al
        inc     edx                     ;EDX --> GC data register
        in      al,dx
        mov     cl,al                   ;save original
        ror     ecx,8                   ;save it in high word

        mov     al,0                    ;set plane 0
        out     dx,al                   ;

        mov     cx,dx                   ;save GC address

        call    GetCRTCPort             ;EDX --> CRTC index register

        in      al,dx
        ror     eax,8                   ;save index

        ; MSK - NOTE: Cirrus docs say that a read from 022h will force read mode 0
        mov     al,022h
        out     dx,al
        inc     edx                     ;EDX --> CRTC data register

        in      al,dx                   ;get latch 0
        mov     [ebx].Latch0,al         ;save in CB structure

        xchg    dx,cx                   ;EDX --> GC data register

        mov     al,1                    ;set plane 1
        out     dx,al

        xchg    dx,cx                   ;EDX --> CRTC data register
        in      al,dx                   ;get latch 1
        mov     [ebx].Latch1,al         ;save in CB structure

        xchg    dx,cx                   ;EDX --> GC data register

        mov     al,2                    ;set plane 2
        out     dx,al

        xchg    dx,cx                   ;EDX --> CRTC data register
        in      al,dx                   ;get latch 2
        mov     [ebx].Latch2,al         ;save in CB structure

        xchg    dx,cx                   ;EDX --> GC data register

        mov     al,3                    ;set plane 3
        out     dx,al

        xchg    dx,cx                   ;EDX --> CRTC data register
        in      al,dx                   ;get latch 3
        mov     [ebx].Latch3,al         ;save in CB structure

        dec     edx                     ;EDX --> CRTC index register
        rol     eax,8                   ; 
        out     dx,al                   ;restore index

        xchg    dx,cx                   ;EDX --> GC data register

        rol     ecx,8                   ;get saved GC[4] value
        mov     al,cl
        out     dx,al                   ;restore GR[4]

        dec     edx                     ;EDX --> GC index register
        rol     eax,8                   ; 
        out     dx,al                   ;restore index

        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret                             ;
EndProc MiniVDD_SaveLatches
;
;
public  MiniVDD_RestoreLatches
BeginProc MiniVDD_RestoreLatches, DOSVM
;
;This routine reverses the latch save that we did prior to restoring the
;latches.  Just restore the states that you saved.
;
;Entry:
;       EBX contains a Windows VM handle (which must be preserved).
;       EBP --> VM's Client Registers.
;Exit:
;       Save anything that you use.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_RestoreLatches", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        push    eax
        push    ebx
        push    ecx
        push    edx
        add     ebx,OurCBDataPointer    ;EBX --> VM's CB data area

        call    MiniVDD_SetLatchBank

        mov     edx,GDC_INDEX           ;EDX --> GC index register
        in      al,dx
        ror     eax,8                   ;save index

        mov     al,004h
        out     dx,al
        inc     edx                     ;EDX --> GC data register
        in      al,dx
        ror     eax,8                   ;save it in high word

        mov     ecx,Vid_PhysA0000

        mov     al,0                    ;set plane 0
        out     dx,al                   ;

        mov     al,[ebx].Latch0         ;Latch 0 value
        mov     [ecx],al

        mov     al,1                    ;set plane 1
        out     dx,al                   ;

        mov     al,[ebx].Latch1         ;Latch 1 value
        mov     [ecx],al

        mov     al,2                    ;set plane 2
        out     dx,al                   ;

        mov     al,[ebx].Latch2         ;Latch 2 value
        mov     [ecx],al

        mov     al,3                    ;set plane 3
        out     dx,al                   ;

        mov     al,[ebx].Latch3         ;Latch 3 value
        mov     [ecx],al

        mov     al,[ecx]                ;load the latches!

        rol     eax,8                   ;get saved GC[4] value
        out     dx,al                   ;restore GR[4]

        dec     edx                     ;EDX --> GC index register
        rol     eax,8                   ; 
        out     dx,al                   ;restore index

        call    MiniVDD_ResetLatchBank

        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret                             ;
EndProc MiniVDD_RestoreLatches
;
;
subttl          Enable and Disable BLTer Register Trapping
page +
public  MiniVDD_EnableTraps
BeginProc MiniVDD_EnableTraps, DOSVM
;
;As stated elsewhere, the VDD needs to set traps on the BLTer registers used
;by the display driver whenever it is using the off-screen memory to
;virtualize VGA graphics mode in the background or in a window.  The reason
;the VDD does this is to receive notification on the first access to a
;BLTer register AFTER returning to the Windows VM.  Then, the VDD switches
;the state of the hardware back to that appropriate for running Windows
;HiRes mode.  In this routine, all you need to do is call the VMM service
;Enable_Global_Trapping for each of the ports that you registered at
;MiniVDD_Device_Init.  Then, the VDD will receive notification at the
;proper time and will switch states properly.
;
;Entry:
;       Nothing assumed.
;Exit:
;       Just Preserve EBX, and ESI.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_EnableTraps", DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        ret                             ;
EndProc MiniVDD_EnableTraps
;
;
public  MiniVDD_DisableTraps
BeginProc MiniVDD_DisableTraps, DOSVM
;
;See comment at EnableTraps.
;
;Entry:
;       Nothing assumed.
;Exit:
;       Just Preserve EBX and ESI.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_DisableTraps", DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        ret                             ;
EndProc MiniVDD_DisableTraps
;
;
subttl          Set Hardware to a Not Busy State
page +
public  MiniVDD_MakeHardwareNotBusy
BeginProc MiniVDD_MakeHardwareNotBusy, DOSVM
;
;Quite often, we need to make sure that the hardware state is not busy
;before changing the MemC mode from the Windows HiRes state to VGA state
;or vice-versa.  This routine allows you to do this (to the best of your
;ability).  You should try to return a state where the hardware BLTer
;isn't busy.
;
;Entry:
;       EAX contains the CRTC owner's VM handle.
;       EBX contains the currently running VM handle.
;       ECX contains the MemC owner's VM handle.
;       EDX contains the CRTC index register.
;Exit:
;       You must save all registers that you destroy except for EAX & ECX.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_MakeHardwareNotBusy", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                Debug_Printf    ":CRTC:%08lX", eax, DEBLEVELMAX
                Debug_Printf    ":MemC:%08lX", ecx, DEBLEVELMAX
                Debug_Printf    ":I:%02lX", edx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        push    edx                     ;save registers that we use for test
        cmp     eax,WindowsVMHandle     ;is CRTC VM Windows?
        jne     MHNBBoardNotBusy        ;nope, skip this entire mess!
;For NVidia, the MemC state does not affect the accelerator, so we never have
;to wait!!
; MSK - TODO - Remove this whole routine!!!!??
;
public  MHNBBoardNotBusy
MHNBBoardNotBusy:
        pop     edx                     ;restore CRTC port number
;
MHNBExit:
        ret                             ;
EndProc MiniVDD_MakeHardwareNotBusy
;
;
subttl          Display Driver Is Being Disabled Notification
page +
public  MiniVDD_DisplayDriverDisabling
BeginProc       MiniVDD_DisplayDriverDisabling, RARE
;
;The display driver is in its Disable routine and is about to set the
;hardware back into VGA text mode.  Since this could either mean that
;the Windows session is ending or that some Windows application is switching
;to a VGA mode to display something full screen (such as MediaPlayer), we
;need to disable our MiniVDD_RestoreRegisters code because we're liable
;to restore a Windows HiRes state when we shouldn't!  Thus, clear the
;DisplayEnabledFlag to prevent this:
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_DisplayDriverDisabling", DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        mov     DisplayEnabledFlag,0    ;don't do a RestoreRegState
        push    ax
        push    dx
;
;   tell the BIOS to program the TV encoder
        call    TellBIOSProgramTV
        pop     ax
        pop     dx       
;
;       if we are about to exit Windows to restart in DOS mode, we need to stop
;       trapping real mode access registers so the BIOS can do a mode set.

MDDDExit:
        ret                             ;
EndProc MiniVDD_DisplayDriverDisabling
;
;
subttl          Virtualize Sequencer Register Extensions
page +
public  MiniVDD_VirtSeqOut
BeginProc MiniVDD_VirtSeqOut, DOSVM
;
;This routine is called when the Sequencer or data register (3C5H) is trapped
;when a VM is being virtualized.  It gives you the opportunity to special
;case virtualization of the Sequencer registers for your particular
;device's needs.
;
;Entry:
;       AL contains the byte to output to port.
;       EBX contains the VM Handle for this Sequencer output.
;       ECX contains the Sequencer index of the port to virtualize.
;       EDX contains the physical register to write or virtualize.
;Exit:
;       AL contains the byte to output to port.
;       Set CY to tell caller that WE handled this Sequencer output.
;       Set NC to tell caller (Main VDD) to handle this Sequencer output
;               in whatever way it normally does
;       EBX, ECX, and EDX should be preserved.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_VirtSeqOut", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                ;
                push    esi
                push    edi
                VxDCall VDD_Get_VM_Info         ;get MemC owner VM handle in ESI for debugging!!!!
                Debug_Printf    ":CRTC:%08lX", edi, DEBLEVELMAX
                Debug_Printf    ":MemC:%08lX", esi, DEBLEVELMAX
                pop     edi
                pop     esi
                ;
                Debug_Printf    ":P:%04lX", edx, DEBLEVELMAX
                Debug_Printf    ":I:%02lX", ecx, DEBLEVELMAX
                pop     eax
                ;
                push    eax
                and     eax,0FFh
                Debug_Printf    ":D:%02lX", eax, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        cmp     cl,006h                 ;is it Sequencer register 6?
        jne     VSOExit                 ;nope, let the main VDD handle it
        push    eax
        push    esi
        push    edi
;
; MSK - TODO - Which way???!!!!
IF 0
        ; Why did Cirrus do this?  Shouldn't we use the current VM?
        VxDCall VDD_Get_VM_Info         ;get MemC owner VM handle in ESI
ELSE ; 0
        mov     esi,ebx
ENDIF ; 0
;
;Save the new state of SR06 here.
;
        add     esi,OurCBDataPointer    ;
        .if (al != 057h)              ;is this the unlock value?
                xor     al,al           ;signal locked
        .endif
        mov     [esi].SeqReg06,al       ;save this state
        pop     edi
        pop     esi
        pop     eax
        
        ; CANOPUS Code
        test    nvCustomer, NVCUSTOMER_CANOPUS
        jz      @f
            cmp     InFullScreen, 0     ; Full screen mode ?
            jne     VSOExit                         ; yes, let VDD do I/O
@@:        
        
;JohnH
                cmp             DoNotSimulate,1                 ;if Windows exit, we should go to hardware
        jz              VSOExit                                 ;let VDD do I/O
; check VM; if we are in Windows, don't bother to do output
; else, we are in DOS, so let VDD update register

                VMMCall Test_Sys_VM_Handle              ;is it the system VM?
        jnz             VSOExit                                 ;no, let VDD do update
;JohnH
        stc                             ;we did the virtualization
        jmp     VSOExit2
;
VSOExit:
        clc                             ;tell Main VDD to handle these normally
VSOExit2:
        ret                             ;
EndProc MiniVDD_VirtSeqOut

page +
public  MiniVDD_VirtSeqIn
BeginProc MiniVDD_VirtSeqIn, DOSVM
;
;This routine is called when the Sequencer or data register (3C5H) is trapped
;when a VM is being virtualized.  It gives you the opportunity to special
;case virtualization of the Sequencer registers for your particular
;device's needs.
;
;Entry:
;       EBX contains the VM Handle for this Sequencer input.
;       ECX contains the Sequencer index of the port to virtualize.
;       EDX contains the physical register to write or virtualize.
;Exit:
;       Set CY to tell caller that we virtualized the port and AL has the value.
;       EBX, ECX, and EDX should be preserved.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_VirtSeqIn", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                ;
                push    esi
                push    edi
                VxDCall VDD_Get_VM_Info         ;get MemC owner VM handle in ESI for debugging!!!!
                Debug_Printf    ":CRTC:%08lX", edi, DEBLEVELMAX
                Debug_Printf    ":MemC:%08lX", esi, DEBLEVELMAX
                pop     edi
                pop     esi
                ;
                Debug_Printf    ":P:%04lX", edx, DEBLEVELMAX
                Debug_Printf    ":I:%02lX", ecx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        cmp     cl,006h                 ;is it Sequencer register 6?
        jne     VSIExit2                ;nope, let the main VDD handle it
        push    esi
        push    edi
;
; MSK - TODO - Which way???!!!!
IF 0
        ; Why did Cirrus do this?  Shouldn't we use the current VM?
        VxDCall VDD_Get_VM_Info         ;get MemC owner VM handle in ESI
ELSE ; 0
        mov     esi,ebx
ENDIF ; 0
;
;Save the new state of SR06 here.
;
        add     esi,OurCBDataPointer    ;
        mov     al,[esi].SeqReg06       ;get this state
        .if (al != 057h)              ;is this the unlock value?
                mov     al,1            ;signal unlocked
        .else
                xor     al,al           ;signal locked
        .endif
        stc                             ;we did the virtualization        
        pop     edi
        pop     esi
        jmp     VSIExit
VSIExit2:
        clc                             ;we did not do virtualization
VSIExit:
        ret                             ;
EndProc MiniVDD_VirtSeqIn
;
;
subttl          Virtualize CRTC Register Extensions
page +
public  MiniVDD_VirtCRTCOut
BeginProc MiniVDD_VirtCRTCOut, DOSVM
;
;This routine is called when the CRTC or data register (3C5H) is trapped
;when a VM is being virtualized.  It gives you the opportunity to special
;case virtualization of the CRTC registers for your particular
;device's needs.
;
;Entry:
;       AL contains the byte to output to port.
;       EBX contains the VM Handle for this CRTC output.
;       ECX contains the CRTC index of the port to virtualize.
;       EDX contains the physical register to write or virtualize.
;Exit:
;       AL contains the byte to output to port.
;       Set CY to tell caller that WE handled this CRTC output.
;       Set NC to tell caller (Main VDD) to handle this CRTC output
;               in whatever way it normally does
;       EBX, ECX, and EDX should be preserved.
;
; MSK - TODO - We should perhaps perform the I/O instead if the current VM is
; the CRTC owner from some registers, or if the current VM is the MemC owner
; for other registers.
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_VirtCRTCOut", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                ;
                push    esi
                push    edi
                VxDCall VDD_Get_VM_Info         ;get MemC owner VM handle in ESI for debugging!!!!
                Debug_Printf    ":CRTC:%08lX", edi, DEBLEVELMAX
                Debug_Printf    ":MemC:%08lX", esi, DEBLEVELMAX
                pop     edi
                pop     esi
                ;
                Debug_Printf    ":P:%04lX", edx, DEBLEVELMAX
                Debug_Printf    ":I:%02lX", ecx, DEBLEVELMAX
                pop     eax
                ;
                push    eax
                and     eax,0FFh
                Debug_Printf    ":D:%02lX", eax, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        push    ebx                     ; save EBX for later
        cmp     cl,19h                  ; CRTC extension register?
        jb      VCO_NotHandled          ; No, do normal processing

        add     ebx,OurCBDataPointer    ; EBX --> VM's CB data area

        cmp     cl,19h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;save current value over save/restore
        mov     [ebx].PerVMData.EXT_Repaint0NV,al
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,1Ah                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;save current value over save/restore
        mov     [ebx].PerVMData.EXT_Repaint1NV,al
                                        ;signal that this register is valid
        mov     [ebx].PerVMData.EXT_Repaint1NVSet,0FFh
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,1Bh                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;save current value over save/restore
        mov     [ebx].PerVMData.EXT_FifoBurst,al
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,1Ch                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;save current value over save/restore
        mov     [ebx].PerVMData.EXT_Repaint3NV,al
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,1Dh                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;save current value over save/restore
        mov     [ebx].PerVMData.EXT_WriteBank,al
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,1Eh                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;save current value over save/restore
        mov     [ebx].PerVMData.EXT_ReadBank,al
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,20h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;save current value over save/restore
        mov     [ebx].PerVMData.EXT_FifoLowWaterMark,al
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,25h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;save current value over save/restore
        mov     [ebx].PerVMData.EXT_ExtraBits,al
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,28h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;save current value over save/restore
        mov     [ebx].PerVMData.EXT_PixelFormat,al
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,29h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;save current value over save/restore
        mov     [ebx].PerVMData.EXT_TVDecimation,al
        jmp     VCO_SpecialExit         ;signal we handled it
;

        ; CANOPUS TV Code
@@:
        test    nvCustomer, NVCUSTOMER_CANOPUS
        jz      skip_canopus_vco1
CANOPUS_VirtCRTCOut:
        cmp     cl, bCanopusTVReg       ; Shadow register?
        jnz     @F                      ; no, check next register
        and     al, NOT TVF_INACTIVE    ; we won't save/restore bit 7 (inactive bit)
                                        ;save current value over save/restore
        mov     [ebx].PerVMData.EXT_CanopusTVReg, al
        jmp     VCO_SpecialExit         ;signal we handled it
skip_canopus_vco1:    

@@:
        cmp     cl,2Ah                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;save current value over save/restore
        mov     [ebx].PerVMData.EXT_OverscanHigh,al
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,2Dh                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;save current value over save/restore
        mov     [ebx].PerVMData.EXT_HorizontalExtra,al
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,30h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;save current value over save/restore
        mov     [ebx].PerVMData.EXT_HWCursorAddress0,al
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,31h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;save current value over save/restore
        mov     [ebx].PerVMData.EXT_HWCursorAddress1,al
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,32h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;save current value over save/restore
        mov     [ebx].PerVMData.EXT_VideoWindow0,al
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,33h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;save current value over save/restore
        mov     [ebx].PerVMData.EXT_VideoWindow1,al
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,39h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;save current value over save/restore
        mov     [ebx].PerVMData.EXT_Interlace,al
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,38h                  ; Is this the real mode control register?
        jnz     @F

        mov     [ebx].PerVMData.EXT_RealModeAccess,al
        jmp     VCO_SpecialExit         ;signal we handled it
@@:
;
; all other extended registers come here
;
        push    eax                     ; save original eax
        xchg    ah,al                   ; Save value to write in ah
        dec     edx                     ; EDX --> CRTC index register
        in      al,dx
        ror     eax,8                   ; save index, data to output moves to al

        mov     ah,al                   ; data to output to AH
        mov     al,cl                   ; index to AL for output
        out     dx,ax                   ; output the data

        rol     eax,8                   ; this puts the result in AH, AL=index
        out     dx,al                   ; restore index
        inc     edx                     ; EDX --> CRTC data register
        pop     eax                     ; restore original eax
VCO_SpecialExit:

; CANOPUS
    test nvCustomer, NVCUSTOMER_CANOPUS
    jz   VCO_Can01
;; When in full screen, always let the VDD output
        cmp     InFullScreen, 0
        jne     VCO_NotHandled

;; If Windows exit, we should go to hardware too...
        cmp     DoNotSimulate,1         
        je      VCO_NotHandled
    
; check VM; if we are in Windows, don't bother to do output
; else, let VDD update register
    VMMCall Test_Sys_VM_Handle              ;is it the system VM?
    jz             VCO_NotHandled           ;yes, let VDD do update (NOTE THIS IS OPPOSITE LOGIC FROM BELOW)
    jmp VCO_Can02    
VCO_Can01:

;JohnH
; check VM; if we are in Windows, don't bother to do output
; else, let VDD update register
                VMMCall Test_Sys_VM_Handle              ;is it the system VM?
        jnz             VCO_NotHandled                  ;no, let VDD do update
;JohnH
VCO_Can02:
        stc                             ; say we did it, so the output isn't done
        jmp     VCOExit

VCO_NotHandled:
        clc                             ;tell caller we didn't do output
VCOExit:
        pop     ebx
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret                             ;
EndProc MiniVDD_VirtCRTCOut
;
;
page +
public  MiniVDD_VirtCRTCIn
BeginProc MiniVDD_VirtCRTCIn, DOSVM
;
;This routine is called when the CRTC or data register (3C5H) is trapped
;when a VM is being virtualized.  It gives you the opportunity to special
;case virtualization of the CRTC registers for your particular
;device's needs.
;
;Entry:
;       EBX contains the VM Handle for this CRTC input.
;       ECX contains the CRTC index of the port to virtualize.
;       EDX contains the physical register to write or virtualize.
;Exit:
;       Set CY to tell caller that we virtualized the port and AL has the value.
;       EBX, ECX, and EDX should be preserved.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_VirtCRTCIn", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                ;
                push    esi
                push    edi
                VxDCall VDD_Get_VM_Info         ;get MemC owner VM handle in ESI for debugging!!!!
                Debug_Printf    ":CRTC:%08lX", edi, DEBLEVELMAX
                Debug_Printf    ":MemC:%08lX", esi, DEBLEVELMAX
                pop     edi
                pop     esi
                ;
                Debug_Printf    ":P:%04lX", edx, DEBLEVELMAX
                Debug_Printf    ":I:%02lX", ecx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        push    ebx                     ; save EBX for later
        cmp     cl,19h                  ; CRTC extension register?
        jb      VCI_NotHandled          ; No, do normal processing

        add     ebx,OurCBDataPointer    ; EBX --> VM's CB data area

        cmp     cl,19h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;read saved value
        mov     al,[ebx].PerVMData.EXT_Repaint0NV
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,1Ah                  ; Shadow register?
        jnz     @F                      ; no, check next register
;During a set mode, the NVidia BIOS first reads 01Ah, and then only changes
;some of the bits. If this is a new VM, then this saved value is 0, which
;will cause the compatible text mode bit to be cleared.  To tell if this
;is a non-initialized value, we have a flag that we set in MiniVDD_VirtCRTCOut
;and in MiniVDD_SaveRegisters.
                                        ; see if value was ever set or saved
        mov     al,[ebx].PerVMData.EXT_Repaint1NVSet
        .if (al == 0)
                mov     al,03Fh                 ; return power on value
        .else
                                                ;read saved value
                mov     al,[ebx].PerVMData.EXT_Repaint1NV
        .endif
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,1Bh                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;read saved value
        mov     al,[ebx].PerVMData.EXT_FifoBurst
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,1Ch                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;read saved value
        mov     al,[ebx].PerVMData.EXT_Repaint3NV
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,1Dh                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;read saved value
        mov     al,[ebx].PerVMData.EXT_WriteBank
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,1Eh                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;read saved value
        mov     al,[ebx].PerVMData.EXT_ReadBank
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,20h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;read saved value
        mov     al,[ebx].PerVMData.EXT_FifoLowWaterMark
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,25h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;read saved value
        mov     al,[ebx].PerVMData.EXT_ExtraBits
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,28h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;read saved value
        mov     al,[ebx].PerVMData.EXT_PixelFormat
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,29h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;read saved value
        mov     al,[ebx].PerVMData.EXT_TVDecimation
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:

;
; CANOPUS TV Code
    test    nvCustomer, NVCUSTOMER_CANOPUS
    jz      VCO_can50
CANOPUS_VirtCRTCIn:
        cmp     cl, bCanopusTVReg       ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;read saved value
;;   One big problem is, that the boot value initial value for a VM is always zero,
;;       and the bios only reads this register!!!
;;       So we use the bit 6 to mark if we have this bit already initialized 
;;       for the current VM or not...
        dec     dx                      ;-- first get bit 7  (inactive bit)
        mov     al, cl
        out     dx, al                  ; set index register (perhaps it's not done now)
        inc     dx                      ; restore dx
        in      al, dx                  ; 
        test    [ebx].PerVMData.EXT_CanopusTVReg, TVF_WINDOWS
        jz      VCI_TvReg               ; is an initial value, use current register state

        and     al, TVF_INACTIVE        ; because we won't save/restore it
                                        ;read saved value
        or      al, [ebx].PerVMData.EXT_CanopusTVReg
        jmp     VCI_SpecialExit         ;signal we handled it

VCI_TvReg:                              ; The VM is new, so we need to initialize the value...
        or      al, TVF_WINDOWS         ; mark value initialized
        push    ax
        and     al, NOT TVF_INACTIVE
        mov     [ebx].PerVMData.EXT_CanopusTVReg, al    ; save and return
        pop     ax
        jmp     VCI_SpecialExit         ;signal we handled it
VCO_can50:
@@:
        cmp     cl,2Ah                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;read saved value
        mov     al,[ebx].PerVMData.EXT_OverscanHigh
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,2Dh                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;read saved value
        mov     al,[ebx].PerVMData.EXT_HorizontalExtra
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,30h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;read saved value
        mov     al,[ebx].PerVMData.EXT_HWCursorAddress0
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,31h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;read saved value
        mov     al,[ebx].PerVMData.EXT_HWCursorAddress1
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,32h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;read saved value
        mov     al,[ebx].PerVMData.EXT_VideoWindow0
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,33h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;read saved value
        mov     al,[ebx].PerVMData.EXT_VideoWindow1
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,39h                  ; Shadow register?
        jnz     @F                      ; no, check next register
                                        ;read saved value
        mov     al,[ebx].PerVMData.EXT_Interlace
        jmp     VCO_SpecialExit         ;signal we handled it
;
@@:
        cmp     cl,38h                  ; Is this the real mode control register?
        jnz     @F

        mov     al,[ebx].PerVMData.EXT_RealModeAccess
        jmp     VCO_SpecialExit         ;signal we handled it
@@:
;
; all other extended registers come here
;
VCI_Physical:
        push    ecx
        push    eax
        dec     edx                     ; EDX --> CRTC index register
        in      al,dx
        ror     eax,8                   ; save index

        mov     al,cl                   ; index to AL for output
        out     dx,al                   ; set the index
        inc     edx                     ; EDX --> CRTC data register
        in      al,dx                   ; get the data register
        mov     ch,al                   ; save value we read
        dec     edx                     ; EDX --> CRTC index register

        rol     eax,8                   ; this puts the result in AH, AL=index
        out     dx,al                   ; restore index
        inc     edx                     ; EDX --> CRTC data register
        pop     eax
        mov     al,ch                   ; restore value we read
        pop     ecx
VCI_SpecialExit:
;JohnH
;               VMMCall Test_Sys_VM_Handle              ;is it the system VM?
;        jnz            VCI_NotHandled                  ;no, let VDD update
;JohnH
        stc                             ; tell caller WE did the input
        jmp     VCOExit

VCI_NotHandled:
        clc                             ;tell caller we didn't do input
VCI_Exit:
        pop     ebx
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret                             ;
EndProc MiniVDD_VirtCRTCIn
;
;
IFDEF NVIDIA_COMMON
subttl          Virtualize I/O To Ports 003D0h-003D3h
page +
public  SimulateNVRead
BeginProc SimulateNVRead
; ebx = OurCBDataPointer
; eax = NV address
; returns
; eax = data
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "SimulateNVRead", DEBLEVELMAX
                pop     eax
                ;
                push    eax
                Debug_Printf    ":NV Address:%08lX", eax, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        .if (eax == NV_PRAMDAC_PLL_COEFF_SELECT)
                mov     eax,[ebx].PerVMData.EXT_NV_PRAMDAC
        .elseif (eax == NV_PRAMDAC_VPLL_COEFF)
                mov     eax,[ebx].PerVMData.EXT_NV_VPLL_COEFF
        .elseif (eax == NV_PRAMDAC_GENERAL_CONTROL)
                mov     eax,[ebx].PerVMData.EXT_NV_PDAC_GEN_CTRL
        .elseif (eax == NV_PFB_CONFIG_0)
                mov     eax,[ebx].PerVMData.EXT_NV_PFB_CONFIG_0
        .else
                ; MSK - TODO - for debugging, we should record what
                ; the hell we were trying to read
                call    Beep
                push    esi
                                        ; if we don't know what it is
                REG_RD32(eax)           ; read the real thing
                pop     esi
        .endif
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret                             ;
EndProc SimulateNVRead
;
;
public  SimulateNVWrite
BeginProc SimulateNVWrite
; ebx = OurCBDataPointer
; eax = NV address
; ecx = data
        ; Write Data to NV register
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "SimulateNVRead", DEBLEVELMAX
                pop     eax
                ;
                push    eax
                Debug_Printf    ":NV Address:%08lX", eax, DEBLEVELMAX
                Debug_Printf    ":Data:%08lX", ecx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        .if (eax == NV_PRAMDAC_PLL_COEFF_SELECT)
                mov     [ebx].EXT_NV_PRAMDAC,ecx
        .elseif (eax == NV_PRAMDAC_VPLL_COEFF)
                mov     [ebx].PerVMData.EXT_NV_VPLL_COEFF,ecx
        .elseif (eax == NV_PRAMDAC_GENERAL_CONTROL)
                mov     [ebx].PerVMData.EXT_NV_PDAC_GEN_CTRL,ecx
        .elseif (eax == NV_PFB_CONFIG_0)
                mov     [ebx].PerVMData.EXT_NV_PFB_CONFIG_0,ecx
        .else
                ; MSK - TODO - for debugging, we should record what
                ; the hell we were trying to write
                call    Beep
                push    esi
                mov     esi,eax         ; put address to write in esi
                                        ; if we don't know what it is
                REG_WR32(esi,ecx)       ; write the real thing
                pop     esi
        .endif
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret                             ;
EndProc SimulateNVWrite
;
;
public  SimulateNVInByte
BeginProc SimulateNVInByte
; ebx = vm handle
; edx = port to read
; returns
; al = data
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "SimulateNVInByte", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                Debug_Printf    ":Port:%08lX", edx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        push    ebx
        push    ecx
        add     ebx,OurCBDataPointer    ; EBX --> VM's CB data area
        mov     cl,[ebx].PerVMData.EXT_RealModeAccess

        test    cl,NVRM_ACCESS_MASK     ; Test if windows are open
        .if (zero?)
;                xor     al,al
;                call    Beep
                        mov     al, 0ffh        ; Don't Beep!!  CANOPUS

        .else
                ;now read the proper register
                and     cl,NVRM_SELECT_MASK
                .if (cl == NVRM_ADDRESS)
                        mov     eax,[ebx].EXT_RealModeAddress
                .elseif (cl == NVRM_DATA)
                        mov     eax,[ebx].EXT_RealModeAddress
                        call    SimulateNVRead
                .elseif (cl == NVRM_DATA32)
                        .if (dx < 003D2h)
                                xor     eax,eax                 ;read 0
                                mov     ax,[ebx].EXT_RealModeData32     ; load latched LSW
                        .else
;                                xor     eax,eax                 ;read 0
;                                call    Beep                    ;only low word works
                                        mov     ax, 0ffh                    ; Don't Beep!!  CANOPUS
                        .endif
                .else
                        mov     eax,02B16D065h          ; return the identifier
                .endif
                mov     cl,dl
                and     cl,003h         ; Do decode of which byte
                shl     cl,3            ; * 8
                shr     eax,cl          ; Put into lower byte
                and     eax,0FFh        ; mask        
        .endif
        pop     ecx
        pop     ebx
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret                             ;
EndProc SimulateNVInByte
;
;
public  SimulateNVInWord
BeginProc SimulateNVInWord
; Seperate SimulateNVInWord exists to allow atomic word reads to occur
; note that atomic dword reads can not occur because Windows 95 splits up
; dword i/o into word accesses.
; ebx = vm handle
; edx = port to read
; returns
; al = data
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "SimulateNVInWord", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                Debug_Printf    ":Port:%08lX", edx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        push    ebx
        push    ecx
        add     ebx,OurCBDataPointer    ; EBX --> VM's CB data area
        mov     cl,[ebx].PerVMData.EXT_RealModeAccess

        test    cl,NVRM_ACCESS_MASK     ; Test if windows are open
        .if (zero?)
                xor     al,al
                call    Beep
        .else
                ;now read the proper register
                and     cl,NVRM_SELECT_MASK
                .if (cl == NVRM_ADDRESS)
                        mov     eax,[ebx].EXT_RealModeAddress
                .elseif (cl == NVRM_DATA)
                        mov     eax,[ebx].EXT_RealModeAddress
                        call    SimulateNVRead
                .elseif (cl == NVRM_DATA32)
                        .if (dx == 003D0h)
                                mov     ax,[ebx].EXT_RealModeData32     ; load latched LSW
                        .else
                                xor     eax,eax                 ;read 0
                                call    Beep                    ;only word reads from 3D0 work
                        .endif
                .else
                        mov     eax,02B16D065h          ; return the identifier
                .endif
                mov     cl,dl
                and     cl,003h         ; Do decode of which word
                shl     cl,3            ; * 8
                shr     eax,cl          ; Put into lower word
                and     eax,0FFFFh      ; mask out word
        .endif
        pop     ecx
        pop     ebx
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret                             ;
EndProc SimulateNVInWord
;
;
public  SimulateNVOutByte
BeginProc SimulateNVOutByte
; ebx = vm handle
; eax = value to be written
; edx = port to write
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "SimulateNVOutByte", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX:Port:%08lX:Data:%08lX", ebx, DEBLEVELMAX
                Debug_Printf    ":Port:%08lX:Data:%08lX", edx, DEBLEVELMAX
                pop     eax
                ;
                push    eax
                Debug_Printf    ":Data:%08lX", eax, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        pushad
        add     ebx,OurCBDataPointer    ; EBX --> VM's CB data area
        mov     cl,[ebx].PerVMData.EXT_RealModeAccess

        test    cl,NVRM_ACCESS_MASK     ; Test if windows are open
        .if (!zero?)
                ;now write the proper register
                and     cl,NVRM_SELECT_MASK
                .if (cl == NVRM_ADDRESS)
                                                ; Get the dword value into eax
                        mov     edi,[ebx].EXT_RealModeAddress
                        mov     cl, dl
                        and     cl, 003h        ; Do decode of which byte
                        shl     cl, 3           ; * 8
                        mov     esi, 0FFh       ; make mask
                        and     eax, esi        ; make sure we only have a byte
                        shl     esi, cl
                        not     esi
                        and     edi, esi        ; Mask out value
                        shl     eax, cl         ; Put byte into proper place
                        or      eax, edi        ; Insert new byte
                                                ; Store new dword value
                        mov     [ebx].EXT_RealModeAddress,eax
                .elseif (cl == NVRM_DATA)
                        call    Beep            ; BIOS should not be writing here
                        ; but it should still write the byte if it is done
                        ; in ax = byte to write
                        ; edx = port addresss
                        ; ebx = OurCBPointer
                        push    eax             ; save byte to write
                        mov     eax,[ebx].EXT_RealModeAddress
                        call    SimulateNVRead
                        mov     edi,eax         ; put original in edi
                        pop     eax             ; restore byte to write

                        mov     cl, dl
                        and     cl, 003h        ; Do decode of which byte
                        shl     cl, 3           ; * 8
                        mov     esi, 0FFh       ; make mask
                        and     eax, esi        ; make sure we only have a word
                        shl     esi, cl
                        not     esi
                        and     edi, esi        ; Mask out value
                        shl     eax, cl         ; Put byte into proper place
                        or      eax, edi        ; Insert new byte

                        ; Write Data to NV register
                        mov     ecx,eax
                        mov     eax,[ebx].EXT_RealModeAddress
                        call    SimulateNVWrite
                .elseif (cl == NVRM_DATA32)
                        ; in al = byte to write
                        ; edx = port addresss
                        ; ebx = OurCBPointer
                        push    eax             ; save byte to write
                        mov     eax,[ebx].EXT_RealModeAddress
                        call    SimulateNVRead
                        mov     edi,eax         ; put original in edi
                        pop     eax             ; restore byte to write

                        mov     di,[ebx].EXT_RealModeData32     ; load latched LSW

                        mov     cl, dl
                        and     cl, 003h        ; Do decode of which byte
                        shl     cl, 3           ; * 8
                        mov     esi, 0FFh       ; make mask
                        and     eax, esi        ; make sure we only have a word
                        shl     esi, cl
                        not     esi
                        and     edi, esi        ; Mask out value
                        shl     eax, cl         ; Put byte into proper place
                        or      eax, edi        ; Insert new byte

                        .if (dx < 003D2h)
                                mov     [ebx].EXT_RealModeData32,ax     ; save latched LSW
                        .else
                                ; Write Data to NV register
                                mov     ecx,eax
                                mov     eax,[ebx].EXT_RealModeAddress
                                call    SimulateNVWrite
                        .endif
                .else                   ; identifier register
                        call    Beep    ; Should not be writing here
                .endif
        .endif
        popad
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret                             ;
EndProc SimulateNVOutByte
;
;
public  SimulateNVOutWord
BeginProc SimulateNVOutWord
; ebx = vm handle
; eax = value to be written
; edx = port to write
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "SimulateNVOutWord", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX:Port:%08lX:Data:%08lX", ebx, DEBLEVELMAX
                Debug_Printf    ":Port:%08lX:Data:%08lX", edx, DEBLEVELMAX
                pop     eax
                ;
                push    eax
                Debug_Printf    ":Data:%08lX", eax, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        pushad
        add     ebx,OurCBDataPointer    ; EBX --> VM's CB data area
        mov     cl,[ebx].PerVMData.EXT_RealModeAccess

        test    cl,NVRM_ACCESS_MASK     ; Test if windows are open
        .if (!zero?)
                ;now write the proper register
                and     cl,NVRM_SELECT_MASK
                .if (cl == NVRM_ADDRESS)
                                                ; Get the dword value into eax
                        mov     edi,[ebx].EXT_RealModeAddress
                        mov     cl, dl
                        and     cl, 003h        ; Do decode of which word
                        shl     cl, 3           ; * 8
                        mov     esi, 0FFFFh     ; make mask
                        and     eax, esi        ; make sure we only have a word
                        shl     esi, cl
                        not     esi
                        and     edi, esi        ; Mask out value
                        shl     eax, cl         ; Put byte into proper place
                        or      eax, edi        ; Insert new word
                                                ; Store new dword value
                        mov     [ebx].EXT_RealModeAddress,eax

                .elseif (cl == NVRM_DATA)
                        call    Beep            ; BIOS should not be writing here
                        ; but it should still write the word if it is done
                        ; in ax = byte to write
                        ; edx = port addresss
                        ; ebx = OurCBPointer
                        push    eax             ; save word to write
                        mov     eax,[ebx].EXT_RealModeAddress
                        call    SimulateNVRead
                        mov     edi,eax         ; put original in edi
                        pop     eax             ; restore word to write

                        mov     cl, dl
                        and     cl, 003h        ; Do decode of which word
                        shl     cl, 3           ; * 8
                        mov     esi, 0FFFFh     ; make mask
                        and     eax, esi        ; make sure we only have a word
                        shl     esi, cl
                        not     esi
                        and     edi, esi        ; Mask out value
                        shl     eax, cl         ; Put byte into proper place
                        or      eax, edi        ; Insert new byte

                        ; Write Data to NV register
                        mov     ecx,eax
                        mov     eax,[ebx].EXT_RealModeAddress
                        call    SimulateNVWrite
                .elseif (cl == NVRM_DATA32)
                        ; in ax = word to write
                        ; edx = port addresss
                        ; ebx = OurCBPointer
                        .if ((dx == 003D1h) || (dx == 003D3h))
                                call    Beep            ; only aligned accesses do anything!
                        .elseif (dx == 003D0h)
                                mov     [ebx].EXT_RealModeData32,ax     ; save latched LSW
                        .else
                                mov     cx,ax
                                shl     ecx,16          ; put word into upper half
                                mov     cx,[ebx].EXT_RealModeData32     ; load latched LSW

                                ; Write Data to NV register
                                mov     eax,[ebx].EXT_RealModeAddress
                                call    SimulateNVWrite
                        .endif
                .else                   ; identifier register
                        call    Beep    ; Should not be writing here
                .endif
        .endif
        popad
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret                             ;
EndProc SimulateNVOutWord
;
;
public  MiniVDD_VirtualRealModeAccess
BeginProc MiniVDD_VirtualRealModeAccess
;
;NVidia has a 32-bit wide port at 003D0h that allows access to the NV register
;space.  This routine virtualizes access to that port.
;NOTE:  The VDD chops up all accesses into bytes. Since our ports support word access,
;               we must remember the first byte until we get the second byte, and put them
;               together to do the actual I/O.
;
;Entry:
;       EAX contains value to be written to port.
;       EBX contains the VM Handle for which this I/O is being done.
;       ECX contains the I/O type flags (see VMM.INC for details).
;       EDX contains the port number.
;Exit:
;       CY clear means that we didn't handle the I/O (let caller do it).
;               -- All registers MUST be preserved in this case.
;       CY set means that we handled the I/O completely.
;               -- Preserve EBX only in this case.
;       EAX contains result of an IN operation.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "SimulateNVOutByte", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                Debug_Printf    ":Port:%08lX", edx, DEBLEVELMAX
                pop     eax
                ;
                push    eax
                Debug_Printf    ":Data:%08lX", eax, DEBLEVELMAX
                Debug_Printf    ":Type:%08lX", ecx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        cmp     edx,03D0h               ;are we interested?
        jb      MVBINotInterested       ;below 03D0h, so no
        cmp     edx,03D3h               ;are we interested?
        ja      MVBINotInterested       ;above 03D3h, so no
        cmp     ecx,BYTE_OUTPUT         ;doing an OUT from a byte?
        je      MVBIByteOutput          ;yes! go handle it!
        cmp     ecx,WORD_OUTPUT         ;doing an OUT from a word?
        je      MVBIWordOutput          ;yes! go handle it!
        cmp     ecx,BYTE_INPUT          ;doing an IN from a byte?
        je      MVBIByteInput           ;yes! go handle it!
        cmp     ecx,WORD_INPUT          ;doing an IN from a word?
        jne     MVBINotInterested       ;nope, we're not interested!
;
public  MVBIWordInput
MVBIWordInput:
        cmp     edx,003D2h              ;do we do the full word
        jle     MVBIWordInputTotal
        call    SimulateNVInByte        ;simulate in byte
        jmp     MVBIWeHandledIt         ;done, (003D4h gets ignored)
public  MVBIWordInputTotal                
MVBIWordInputTotal:
        call    SimulateNVInWord        ;simulate in word
        jmp     MVBIWeHandledIt         ;and pretend that we handled it
;
public  MVBIWordOutput
MVBIWordOutput:
        cmp     edx,003D2h
        jle     MVBIInRange
        call    SimulateNVOutByte       ;only do the byte at 003D3h
        jmp     MVBIWeHandledIt         ;done, (003D4h gets ignored)
MVBIInRange:
        call    SimulateNVOutWord       ;simulate out word
        jmp     MVBIWeHandledIt         ;and pretend that we handled it

;--------------------------------------------------------------------------------------------
;BYTE INPUT of real mode access register (3d0-3d3)
;the VDD splits up our word accesses into bytes.
;input word and return the appropriate byte.
;JohnH
public  MVBIByteInput
MVBIByteInput:

; CANOPUS Check for TV fullscreen
        test    nvCustomer, NVCUSTOMER_CANOPUS
        jz      MVBIBI_can01
        
            cmp InFullScreen, 0         ; Full screen mode ?
            jne @f

        ;If we are not windowed, we may need to do actual I/O
        VMMCall Test_Sys_VM_Handle              ;is it the system VM?
        jnz     SimNvInByte             ;No, simulate (we are windowed)
        jmp @f
MVBIBI_can01:        

        cmp             DoNotSimulate,1                 ;if Windows exit, we should go to hardware
        je              @f              
;If we are not windowed, we may need to do actual I/O
        VMMCall Test_Sys_VM_Handle              ;is it the system VM?
        jz              SimNvInByte                             ;yes, simulate (we are windowed)
@@:     push    dx
;do the input and return the byte referenced
                push    dx
        and             dl,0feh                                 ;input word (from even port)
        in              ax,dx
        pop             dx
        bt              dx,0                                    ;high or low byte?
        jnc             ByteInDone
        shr             ax,8
ByteInDone:
                pop             dx
        jmp             MVBIWeHandledIt                 ;return with word in ax if second byte
SimNvInByte:
        call    SimulateNVInByte        ;simulate in byte
        jmp             MVBIWeHandledIt
        
;--------------------------------------------------------------------------------------------
;BYTE OUTPUT of real mode access register (3d0-3d3)
;the VDD splits up our word accesses into bytes.
;We need to check for consecutive accesses to a word register and put the bytes back together.
;We also need to save valid real-mode access sequences to the per-VM data area.
;The sequence is: 
;       write access mode (address) at CRTC 38.
;       write NV ptr at 3d2, 3d0. 
;       write access mode (data) at CRTC 38
;       write NV data at 3d2, 3d0. 
;We must save the address and data so we can check for interesting addresses
;and write the data to the per-VM data area.
public  MVBIByteOutput
MVBIByteOutput:

        ; CANOPUS TV Check
        test    nvCustomer, NVCUSTOMER_CANOPUS
        jz      MVBIBO_can01

            cmp     InFullScreen, 0             ; Full screen mode ?
            jne     @f
        cmp     DoNotSimulate,1     ;if Windows exit, we should go to hardware
        je      @f              
        VMMCall Test_Sys_VM_Handle  ;is it the system VM?
            jnz     SimNvOutByte                ;No, simulate (we are windowed)
        jmp     @f

MVBIBO_can01:
        cmp             DoNotSimulate,1                 ;if Windows exit, we should go to hardware
        je              @f              
        VMMCall         Test_Sys_VM_Handle              ;is it the system VM?
        jz              SimNvOutByte                    ;yes, simulate (we are windowed)
;If we are not windowed, we may need to do actual I/O
@@:     push    ax
        push    cx
        push    dx
        bt              dx,0                                    ;second byte (possibly)
        jnc             FirstByteOut                    ;jmp if even
        cmp             dx,LastByteOutAddr              ;second byte of same register?
        jnz             ByteOutDone                             ;no, operation is useless                

;ODD address. second byte of the word
                shl             ax,8
        mov             al,LastByteOut
                dec             dx                                              ;back to even addr
        out             dx,ax                                   ;do I/O
;save data in temp array until we have address and data
                push    ebx
                xor             ebx,ebx
                mov             bl,rmState
                cmp             bl,3
        ja              @f                                              ;something is wrong
                shl             bx,1                                    ;word index
                mov             word ptr rmArray[ebx],ax        ;save it, address in low dword, data in high dword
                shr             bx,1
@@:             pop             ebx
        cmp             rmState,3                               ;all 4 words done? (data must be after address)
        jne             ByteOutDone                             ;no, wait for more

;we have all 8 bytes of address/data. save data in register data area
        mov             rmState,0                               ;start over
                ; bx = vm handle
                call    SaveNVPrivReg                   ;sort it out and write it to per VM data area
                xor             ax,ax
        mov             LastByteOutAddr,ax              ;clear out address to end cycle
        mov             LastByteOut,al
                jmp             ByteOutDone        

FirstByteOut:
;EVEN address. check for address/data sequence
;set state: 0=lsw of address, 1=msw of address, 2=lsw of data, 3=msw of data
        call    getAccessState
                cmp             cl,3                                    ;NV ptr?
                jne             @f
        mov             rmState,0                               ;flag address sequence
                jmp             mswlsw                                  ;first or second word?
@@:             cmp             cl,7
                je              @f
                cmp             cl,5
                jne     ignore
@@:             mov             rmState,2                               ;flag data sequence
                jmp             mswlsw
ignore:
                mov             rmState,4                               ;ignore
                jmp             SaveEven

;check MSW or LSW                        
;we're assuming LSW comes first. Can this get us in trouble? rmState is used to index into the array where we save it.
mswlsw: cmp             dx,3d0h                                 ;lsw?
        jz              SaveEven
                inc             rmState                                 ;msw, bump state

;save data and port until next byte comes thru
SaveEven:
                inc     dx
                mov             LastByteOutAddr,dx
        mov             LastByteOut,al
ByteOutDone:
                pop             dx
                pop             cx
        pop             ax
        jmp             MVBIWeHandledIt
        
SimNvOutByte:
        call    SimulateNVOutByte       ;simulate out byte
;
public  MVBIWeHandledIt
MVBIWeHandledIt:
        stc                             ;return that we handled it
        jmp     MVBIExit                ;and return IN data in AL/AX to caller
;
public  MVBINotInterested
MVBINotInterested:
        clc                             ;return to let caller handle it!
;
MVBIExit:
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret                             ;
EndProc MiniVDD_VirtualRealModeAccess
;--------------------------------------------------------------------------------------------
;
;               get the access state bits for realmode access registers
;EXIT:  cl = access state
BeginProc               GetAccessState
                push    dx
                push    ax
        ;
        ; CANOPUS TV Check
        test    nvCustomer, NVCUSTOMER_CANOPUS
        jz      GAS_direct                
         
        VMMCall Test_Sys_VM_Handle      ;is it the system VM?
        jz      GAS_direct              ;yes, get the current state

;// We also need to virtualize this register... ;)
GAS_virtual:
        push    ebx
        add     ebx,OurCBDataPointer    ; EBX --> VM's CB data area
        mov     cl, [ebx].PerVMData.EXT_RealModeAccess
        pop     ebx
        and     cl, 07h
        jmp     GAS_end

GAS_direct:
                
        call    GetCRTCPort
        in              al,dx           ;read crtc index
        mov             ch,al
        mov             al,38h          ;access bits in reg 38h
        out             dx,al
        inc             dx
        in              al,dx           ;read access state
                and             al,7
                mov             cl,al           ;save access state for return
        mov             al,ch           ;restore crtc index
        dec             dx
        out             dx,al           ;restore crtc index
        
GAS_end:        
        pop             ax
        pop             dx
        ret        
EndProc                 GetAccessState

;--------------------------------------------------------------------------------------------
;               save data written to critical NV register in the per-VM CB
;Entry: EBX = VM handle
BeginProc               SaveNVPrivReg
                push    ebx
        add     ebx,OurCBDataPointer            ;EBX --> CRTC's CB data
;if the address is one of our 4 interesting registers, save the data
                mov             eax, dword ptr NvData
                cmp             dword ptr NvPtr,NV_PRAMDAC_VPLL_COEFF
        jne             @f
        mov     [ebx].PerVMData.EXT_NV_VPLL_COEFF,eax
                jmp             nv_save_done
@@:             cmp             dword ptr NvPtr,NV_PRAMDAC_PLL_COEFF_SELECT
                jne             @f
        mov     [ebx].PerVMData.EXT_NV_PRAMDAC,eax
                jmp             nv_save_done
@@:             cmp             dword ptr NvPtr,NV_PRAMDAC_GENERAL_CONTROL
                jne             @f
        mov     [ebx].PerVMData.EXT_NV_PDAC_GEN_CTRL,eax
                jmp             nv_save_done
@@:             cmp             dword ptr NvPtr,NV_PFB_CONFIG_0
                jne             nv_save_done
        mov     [ebx].PerVMData.EXT_NV_PFB_CONFIG_0,eax
      
nv_save_done:
                pop             ebx
                ret
EndProc                 SaveNVPrivReg

;
ENDIF ; NVIDIA_COMMON
subttl          Save Message Mode State For NVidia Extended Registers
page +
public  MiniVDD_SaveMessageModeState
BeginProc MiniVDD_SaveMessageModeState, RARE
;
;This routine allows the MiniVDD to copy virtualized states and save them in
;a special Message Mode state structure.  When this routine is called, the
;Main VDD has executed a totally virtualized mode change to the blue-screen
;Message Mode state (ie: Mode 3 in SBCS Windows and Mode 12H in Far East
;Windows).  Therefore, no screen changes are evident but the states have
;been saved in the CB data structure for the Windows VM (since the virtualized
;mode change was done when the Windows VM was the "Currently executing VM".
;Thus, when this routine is called, we have the Message Mode state for all
;of the NVidia Extension Registers saved in the CB data structure for the
;Windows VM.  We simply copy this state into the special Message Mode state
;data structure.
;
;Entry:
;       EBX --> Windows VM.
;       DS --> Windows VM's BIOS data area.
;Exit:
;       Save everything that you use!
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_SaveMessageModeState", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        push    eax                     ;
        push    ebx                     ;
        push    ecx                     ;
        push    edx                     ;
;       push    edi                     ;

        cld                             ;make sure direction is set forward
        add     ebx,OurCBDataPointer    ; EBX --> VM's CB data area
        VMMCall _lmemcpy, <OFFSET32 MessageModeCBData, ebx, size PerVMData>

        mov     MsgModeInitDone,0ffh    ;flag that we're done collecting states
;
SMMSExit:
;       pop     edi                     ;restore saved registers
        pop     edx                     ;
        pop     ecx                     ;
        pop     ebx                     ;
        pop     eax                     ;
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret                             ;we're done
EndProc MiniVDD_SaveMessageModeState
;
;
subttl          Save Forced Planar State For NVidia Extended Registers
page +
public  MiniVDD_SaveForcedPlanarState
BeginProc MiniVDD_SaveForcedPlanarState, RARE
;
;This routine allows the MiniVDD to copy virtualized states and save them in
;a special Forced Planar state structure.  When this routine is called, the
;Main VDD has executed a totally virtualized mode change to the planar state
;(ie: Mode 12H).  Therefore, no screen changes are evident but the states have
;been saved in the CB data structure for the Windows VM (since the virtualized
;mode change was done when the Windows VM was the "Currently executing VM".
;Thus, when this routine is called, we have the planar state for all
;of the NVidia Extension Registers saved in the CB data structure for the
;Windows VM.  We simply copy this state into the special Planar State
;data structure.
;
;Entry:
;       Nothing assumed.
;Exit:
;       Save everything that you use!
;
;The source of our state save copy is the Windows VM's CB data structure:
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_SaveForcedPlanarState", DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        push    eax                     ;
        push    ebx                     ;
        push    ecx                     ;
        push    edx                     ;
;       push    edi                     ;

        cld                             ;make sure direction is set forward
        add     ebx,OurCBDataPointer    ; EBX --> VM's CB data area
        VMMCall _lmemcpy, <OFFSET32 ForcedPlanarCBData, ebx, size PerVMData>

;       pop     edi                     ;restore saved registers
        pop     edx                     ;
        pop     ecx                     ;
        pop     ebx                     ;
        pop     eax                     ;
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret                             ;we're done
EndProc MiniVDD_SaveForcedPlanarState
;
;
subttl          Return Total Memory Size on Card
page +
public  MiniVDD_GetTotalVRAMSize
BeginProc MiniVDD_GetTotalVRAMSize, DOSVM
;
;Entry:
;       EBX contains the Current VM Handle (which is also the CRTC owner's
;       VM Handle).
;       EBP --> VM's Client Registers.
;Exit:
;       CY is returned upon success.
;       All registers (except ECX) must be preserved over the call.
;       ECX will contain the total VRAM size in bytes.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_GetTotalVRAMSize", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        push    eax
        push    esi                     ; Uses REG_RD32 macros

        mov     ecx,800000h             ; default = 8M

        REG_RD32(NV_PFB_BOOT_0)  
        test    ax,40h
        jnz     @f        
        shr     ecx,1                   ; 4M?
        test    ax,4
        jnz     @f
        shr     ecx,1                   ; must be 2M
@@:     mov     TotalMemorySize,ecx     ; probably won't use this
        stc                             ;
        pop     esi
        pop     eax
        ret                             ;return to caller
EndProc MiniVDD_GetTotalVRAMSize
;
;
subttl          Return The Current Write Bank
page +
public  MiniVDD_GetCurrentBankWrite
BeginProc MiniVDD_GetCurrentBankWrite, DOSVM
;
;Entry:
;       EBX contains the VM Handle (Always the "CurrentVM").
;       EBP --> Client Register structure for VM
;Exit:
;       CY is returned upon success.
;       All registers (except EDX & EAX) must be preserved over the call.
;       EDX will contain the current write bank (bank A) set in hardware.
;
;NOTE: We really don't care whether these banks are configured as write or
;      read banks.  The reason that we use this terminology is because the
;      VESA standard defines two "windows" that can be set separately.
;      This routine gives back the first window.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_GetCurrentBankWrite", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
IFDEF FAST_HIRES_SAVE_RESTORE
        xor     edx,edx
ELSE ; FAST_HIRES_SAVE_RESTORE
        call    GetCRTCPort             ;EDX --> CRTC index register
        in      al,dx                   ;get current index register value
        ror     eax,8                   ;save it in top byte of EAX
        mov     al,01Dh                 ;this is index for bank register A
        out     dx,al                   ;
        inc     dl                      ;EDX --> CRTC data register
        in      al,dx                   ;
        dec     dl                      ;EDX --> CRTC index register
        shr     al,1                    ;convert from 32KB banks to 64KB banks
        rol     eax,8                   ;restore previous contents of CRTC index
        out     dx,al                   ;
        movzx   edx,ah                  ;return bank in EDX register
ENDIF ; FAST_HIRES_SAVE_RESTORE
;
MGBWExit:
        stc                             ;return success to caller
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret                             ;
EndProc MiniVDD_GetCurrentBankWrite
;
;
subttl          Return The Current Read Bank
page +
public  MiniVDD_GetCurrentBankRead
BeginProc MiniVDD_GetCurrentBankRead, DOSVM
;
;Entry:
;       EBX contains the VM Handle (Always the "CurrentVM").
;       EBP --> Client Register structure for VM
;Exit:
;       CY is returned upon success.
;       All registers (except EDX & EAX) must be preserved over the call.
;       EDX will contain the current read bank (bank A) set in hardware.
;
;NOTE: We really don't care whether these banks are configured as write or
;      read banks.  The reason that we use this terminology is because the
;      VESA standard defines two "windows" that can be set separately.
;      This routine gives back the second window.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_GetCurrentBankRead", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
IFDEF FAST_HIRES_SAVE_RESTORE
        xor     edx,edx
ELSE ; FAST_HIRES_SAVE_RESTORE
        call    GetCRTCPort             ;EDX --> CRTC index register
        in      al,dx                   ;get current index register value
        ror     eax,8                   ;save it in top byte of EAX
        mov     al,01Eh                 ;set to extensions register index
        out     dx,al                   ;
        inc     dl                      ;EDX --> CRTC data register
        in      al,dx                   ;get banking register
        dec     dl                      ;EDX --> CRTC index register
        shr     al,1                    ;convert from 32KB banks to 64KB banks
        rol     eax,8                   ;restore previous contents of CRTC index
        out     dx,al                   ;
        movzx   edx,ah                  ;return bank value in EDX
ENDIF ; FAST_HIRES_SAVE_RESTORE
;
MGBRExit:
        stc                             ;return success to caller
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret                             ;
EndProc MiniVDD_GetCurrentBankRead
;
;
subttl          Return the Bank Size
page +
public  MiniVDD_GetBankSize
BeginProc MiniVDD_GetBankSize, DOSVM
;
;Entry:
;       EBX contains the VM Handle (Always the "CurrentVM").
;       ECX contains the BIOS mode number that we're currently in.
;       EBP --> Client Register structure for VM
;Exit:
;       CY is returned upon success.
;       All registers (except EDX & EAX) must be preserved over the call.
;       EDX will contain the current bank size.
;       EAX will contain the physical address of the memory aperture or
;               zero to indicate VRAM at A000H.
;
;Our bank size (for HiRes save/restore purposes) is 64K.
;
; MSK - TODO - should we instead return the linear address with a size that is huge?!!!
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_GetBankSize", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                Debug_Printf    ":Mode:%08lX", ecx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
IFDEF FAST_HIRES_SAVE_RESTORE
        mov     edx,TotalMemorySize
;        call    GetLinearAddress        ;indicate where linear frame buffer is
        mov     eax,_fbAddr
ELSE ; FAST_HIRES_SAVE_RESTORE
        mov     edx,64*1024             ;assume 4K banks
        xor     eax,eax                 ;indicate VRAM is at A000H
ENDIF ; FAST_HIRES_SAVE_RESTORE
;
MGBSExit:
        stc                             ;return success to caller
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret                             ;
EndProc MiniVDD_GetBankSize
;
;
subttl          Prepare For a HiRes Screen Save/Restore
page +
public  MiniVDD_CheckScreenSwitchOK
BeginProc MiniVDD_CheckScreenSwitchOK, DOSVM
;
;Entry:
;       EBX contains the VM Handle (Always the "CurrentVM").
;       ECX contains the BIOS mode number that we're currently in.
;       EBP --> Client Register structure for VM
;Exit:
;       CY is returned if mode can not be switched.
;       All registers must be preserved over the call.
;
;Our bank size (for HiRes save/restore purposes) is 64K.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_CheckScreenSwitchOK", DEBLEVELMAX
                pop     eax
                push    eax
                Debug_Printf    ":VESA:%08lX", eax, DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                Debug_Printf    ":Mode:%08lX", ecx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
                clc                     ; return OK

        ret                            
EndProc MiniVDD_CheckScreenSwitchOK
;
;
public  MiniVDD_PreHiResSaveRestore
BeginProc MiniVDD_PreHiResSaveRestore, DOSVM
;
;Entry:
;       EBX contains the VM Handle (Always the "CurrentVM").
;       ECX contains the BIOS mode number that we're currently in.
;       EBP --> Client Register structure for VM
;Exit:
;       CY is returned upon success.
;       All registers must be preserved over the call.
;
;Our bank size (for HiRes save/restore purposes) is 64K.
;
; MSK - TODO - eliminate?
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_PreHiResSaveRestore", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                Debug_Printf    ":Mode:%08lX", ecx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
; MSK - TODO - we probably need to call save registers here and restore registers in PostHiResSaveRestore if
; they aren't called automatically.  Even if the VDD sets the VBE mode again for the DOS box, it will not
; be able to do the refresh rate.  This only maters if the app forced its own refresh rate!
        stc                             ;return success to caller
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret                             ;
EndProc MiniVDD_PreHiResSaveRestore
;
;
subttl          Clean Up After a HiRes Screen Save/Restore
page +
public  MiniVDD_PostHiResSaveRestore
BeginProc MiniVDD_PostHiResSaveRestore, DOSVM
;
;Entry:
;       EBX contains the VM Handle (Always the "CurrentVM").
;       ECX contains the BIOS mode number that we're currently in.
;       EBP --> Client Register structure for VM
;Exit:
;       CY is returned upon success.
;       All registers must be preserved over the call.
;
;Our bank size (for HiRes save/restore purposes) is 64K.
;
; MSK - TODO - eliminate?
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_PostHiResSaveRestore", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                Debug_Printf    ":Mode:%08lX", ecx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        stc                             ;return success to caller
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret                             ;
EndProc MiniVDD_PostHiResSaveRestore
;
;
subttl          Set the Bank During a HiRes Save/Restore
page +
public  MiniVDD_SetBank
BeginProc MiniVDD_SetBank, DOSVM
;
;Entry:
;       EAX contains the read bank to set.
;       EDX contains the write bank to set.
;       EBX contains the VM Handle (Always the "CurrentVM").
;       EBP --> Client Register structure for VM
;Exit:
;       CY is returned upon success.
;       All registers must be preserved over the call.
;       EDX will contain the current write bank (bank A) set in hardware.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_SetBank", DEBLEVELMAX
                Debug_Printf    ":VM:%08lX", ebx, DEBLEVELMAX
                Debug_Printf    ":RBank:%08lX", eax, DEBLEVELMAX
                Debug_Printf    ":WBank:%08lX", edx, DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
IFNDEF FAST_HIRES_SAVE_RESTORE
        push    ecx                     ;save this over call
        push    edx                     ;
        push    eax                     ;
;JohnH - need to unlock or we can't change banks!
        mov     cl,al                   ;get read bank to set into CL
        mov     ch,dl                   ;get write bank value in CH
        call    GetCRTCPort             ;EDX --> CRTC index register
        in      al,dx                   ;get current index register value
        ror     eax,8                   ;save it in top byte of EAX

;               unlock_extension_regs
                call    unlock_ext
                
;
        mov     ah,ch                   ;get write bank value into AH
        shl     ah,1                    ;convert from 64KB banks to 32KB banks
        mov     al,01Dh                 ;set CRTC index to register 01Dh
        out     dx,ax                   ;set the write bank

        mov     ah,cl                   ;get read bank value into AH
        shl     ah,1                    ;convert from 64KB banks to 32KB banks
        mov     al,01Eh                 ;set CRTC index to register 01Eh
        out     dx,ax                   ;set the read bank
;
                call    restore_lock

        rol     eax,8                   ;restore GCR index register
        out     dx,al                   ;
        pop     eax                     ;
        pop     edx                     ;
        pop     ecx                     ;restore saved ECX
ENDIF ; FAST_HIRES_SAVE_RESTORE
        stc                             ;return success to caller
        ret                             ;
EndProc MiniVDD_SetBank
;
;
subttl          Temperary debug routines
page +
public  MiniVDD_PreCRTCModeChange
BeginProc MiniVDD_PreCRTCModeChange, DOSVM
;
;Entry:
;Exit:
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_PreCRTCModeChange", DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG

IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret
EndProc MiniVDD_PreCRTCModeChange
;
;
;       RM callback after a mode switch
;Entry: parameter on stack. If non-zero, the display has changed (e.g. from CRT to TV)
public  MiniVDD_PostCRTCModeChange
BeginProc MiniVDD_PostCRTCModeChange, DOSVM
;
;Entry:
;Exit:
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_PostCRTCModeChange", DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG

;       We need to indicate the NVIDIA drivers are running, not VGA.DRV.
        mov     DisplayEnabledFlag,0ffh

        mov     eax,[esp+4]
        cmp     eax,1                   ; check parameter for display change
        jne     @f
        mov     DisplayChanged,1 ; flag it for PostHiresToVga
@@:
        ; signal BIOS not to change encoder while in hires
        call    TellBIOSNotProgramTV

;       We need to save registers after the resource manager does a mode change.
;       or else the VDD will restore the wrong registers (i.e., our extended registers). This can happen when closing
;       a windowed DOS box, or ending a full-srcreen app such as an AVI player.
;       This wasn't a problem when we used the BIOS for mode sets, because the I/O got trapped and the VDD called SaveRegisters.
        push    ebx
        mov     ebx,WindowsVMHandle
        call    MiniVDD_SaveRegisters
        pop     ebx
        
        ret
EndProc MiniVDD_PostCRTCModeChange
;
;
subttl          Handle refresh rate support for VESA setmode calls
page +
public MiniVDD_VESASupport
BeginProc MiniVDD_VESASupport
;
;
; MiniVDD_VESASupport
;       Our VBE handler. It processes non-refresh set mode calls and turns them
;       into refresh rate calls.
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_VESASupport", DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        pushad
IFDEF INCLUDE_REFRESH

; MSK - TODO - Add support for doing VESA calls in protected mode using VBE 3.0!!!
        mov     ax,[ebp].Client_Word_Reg_Struc.Client_AX
        cmp     ax,VESA_SET_MODE        ; Is this a Set Mode call?
        je      VESASupportSetMode
ifdef VESA_DDC
        cmp     ax,4F15h                ; Is this a VBE/DDC call?
        je      VESADDC
endif ; VESA_DDC
        jmp     VESASupportPassDown    ; Pass call on down

VESASupportSetMode:
        mov     al,InRefreshSetMode     ; check if we are already doing this
        or      al,al
        jnz     VESASupportPassDown     ; just pass it down if we are

;NVidia uses VBE 3.0 to set the refresh rate.
;First see if this is a refresh rate aware set mode and just pass it on if it is.
        mov     ax,[ebp].Client_Word_Reg_Struc.Client_BX
        test    ax,00800h
        jnz     VESASupportPassDown

;Is this the Windows VM or a DOS VM?
        cmp     ebx,WindowsVMHandle
        jne     VESASupportVM

        ;Determine current windows refresh rate setting
;The Main VDD has already retrieved the Refresh_Rate= and X_Resolution entries
;from SYSTEM.INI and will return those values if the registry has nothing
;better to offer.
;
; MSK - TODO - Add DDC/EDID refresh rate support!!!
; MSK - TODO - Add GTF refresh rate support!!!!
        mov     eax,OFFSET32 DisplayInfoStructure
        mov     ecx,SIZE DISPLAYINFO    ;pass size in ECX, address in EAX
        mov     [eax].diHdrSize,cx      ;fill in this entry
        push    eax                     ;save EAX --> DisplayInfoStructure
        VxDCall VDD_Get_DISPLAYINFO     ;get information from the VDD
        pop     edi                     ;restore EAX --> DisplayInfoStructure
        test    [edi].diInfoFlags,MONITOR_INFO_DISABLED_BY_USER OR \
                        MONITOR_INFO_NOT_VALID OR REGISTRY_RESOLUTION_NOT_VALID

; until BIOS correctly supports refresh, bypass this
IFDEF SUPPORT_VESA_REFRESH
        jnz     VESASupportPassDown     ;we are not to set the refresh rate!
ELSE
                jmp             VESASupportPassDown
ENDIF

;
;Since this is a normal setmode, tack on the refresh rate info
;
; MSK - TODO - make sure that we handle the fact that we can't call this until after
; DeviceInit, any set modes before then should just be regular
;
;First, dispatch to the correct resolution handler:
;
        movzx   edx,[edi].diYRes        ;
        or      edx,edx                 ;any Y-resolution in the structure?
        jz      VESASupportPassDown     ;nope, blow out of here!

; Determine if match between resolution and color depth with mode being set
        Push_Client_State
        VMMcall Begin_Nest_V86_Exec

        ; load new values
        mov     ax,[ebp].Client_Word_Reg_Struc.Client_SP
        sub     ax,size VBE_ModeInfoBlock
        mov     [ebp].Client_Word_Reg_Struc.Client_SP,ax
        mov     [ebp].Client_Word_Reg_Struc.Client_DI,ax
        mov     ax,[ebp].Client_Reg_Struc.Client_SS
        mov     [ebp].Client_Word_Reg_Struc.Client_ES,ax
        mov     ax,04F01h
        mov     [ebp].Client_Word_Reg_Struc.Client_AX,ax
        ; copy mode number to cx
        mov     ax,[ebp].Client_Word_Reg_Struc.Client_BX
        and     ax,001FFh
        mov     [ebp].Client_Word_Reg_Struc.Client_CX,ax

        ; copy block to VM stack
        Client_Ptr_Flat esi, ES, DI, USES_EAX

        .if (esi == -1)         ; -1 if error
                mov     ax, TRUE                ; assume its GUI as default in this case
        .else
                ; get the mode info
                mov     eax,010h
                VMMCall Exec_Int

                mov     ax,FALSE                ;assume no match?
                movzx   edx,[edi].diYRes        ;load y resolution
                .if (dx == [esi].VBE_ModeInfoBlock.YResolution)
                        movzx   edx,[edi].diXRes        ;load x resolution
                        .if (dx == [esi].VBE_ModeInfoBlock.XResolution)
                                movzx   edx,[edi].diBpp         ;load bpp
                                .if (dl == [esi].VBE_ModeInfoBlock.BitsPerPixel)
                                        mov     ax,TRUE                 ; do we match?
                                .endif
                        .endif
                .endif
        .endif

        VMMCall End_Nest_Exec
        Pop_Client_State

        cmp     ax,TRUE
        jne     VESASupportPassDown

        movzx   edx,[edi].diYRes        ;reload y resolution
        mov     eax,0                   ;start at 480
        cmp     edx,480                 ;running 640x480?
        je      MVS_Common              ;yes, go handle it
        inc     eax
        cmp     edx,600                 ;running 800x600?
        je      MVS_Common              ;yes, go handle it
        inc     eax
        cmp     edx,768                 ;running 1024x768?
        je      MVS_Common              ;yes, go handle it
        inc     eax
;
public  MVS_Common
MVS_Common:
        shl     eax,2
        mov     edx,refreshRates[eax]
        mov     eax,[edx].RefreshRateStruct.VetricalRefreshRate
MVS_Loop:
        movzx   esi,[edi].diRefreshRateMax
        cmp     eax,esi
        jg      MVS_NextRate
        test    [edi].diInfoFlags,REFRESH_RATE_MAX_ONLY
                                        ;did we not get horizontal rate data?
        jnz     MVS_MaxOnly             ;nope, do simplied method
        movzx   eax,[edi].diHighHorz    ;get high horizontal value into EAX
        jz      MVS_SkipTest
        cmp     [edx].RefreshRateStruct.HorizontalRefreshRate,eax
        jg      MVS_NextRate
MVS_SkipTest:
        movzx   eax,[edi].diLowHorz     ;get low horizontal value into EAX
        jz      MVS_MaxOnly
        cmp     [edx].RefreshRateStruct.HorizontalRefreshRate,eax
        jl      MVS_NextRate
MVS_MaxOnly:
        jmp     MVS_Found
;
MVS_NextRate:
        add     edx,size RefreshRateStruct
        mov     eax,[edx].RefreshRateStruct.VetricalRefreshRate
        jnz     MVS_Loop
;
        sub     edx,size RefreshRateStruct
;
MVS_Found:
        lea     esi,[edx].RefreshRateStruct.crtcInfoBlockForced
        ;
        jmp     VESASupportCommon

VESASupportVM:
        
IFDEF DOS_REFRESH
        ; scan for mode
        mov     edi,OFFSET32 DefaultRefreshModes
        ; load mode number
        mov     ax,[ebp].Client_Word_Reg_Struc.Client_BX
        mov     ecx,DefaultRefreshCount
        or      ecx,ecx
        jz      VESASupportPassDown
        repne   scasw
        jne     VESASupportPassDown
        inc     ecx                     ; Back up one
        mov     edi,DefaultRefreshCount
        sub     edi,ecx                 ; Convert to index
        mov     eax,TYPE CRTCInfoBlock
        mul     edi
        mov     esi,eax
        add     esi,OFFSET32 DefaultRefreshTable
ELSE ; DOS_REFRESH
        jmp     VESASupportPassDown    ; Pass call on down
ENDIF ; DOS_REFRESH
       

VESASupportCommon:
IFDEF OLD_VESA30_REFRESH
        ; Find closest match of a frequency
        mov     edi,OFFSET32 ClockFrequencies
        mov     ax,word ptr [edi]
        mov     dx,ax                   ; initialize dx in case of empty list
        mov     cx,07FFFh               ; biggest possible distance
        .while   (ax != 0FFFFh)
                sub     ax,[esi].CRTCInfoBlock.PhysDotClock
                .if (SWORD PTR ax < 0)
                        neg     ax
                .endif
                .if (ax < cx)
                        mov     cx,ax
                        mov     dx,word ptr [edi]
                .endif
                inc     edi
                inc     edi
                mov     ax,word ptr [edi]
        .endw

        ; Did we find the closest clock?
        cmp     dx,0FFFFh
        je      VESASupportPassDown
ELSE ; OLD_VESA30_REFRESH
        ; Call VESA function 0C to get closest clock frequency to what we want
        Push_Client_State
        VMMcall Begin_Nest_V86_Exec

        ; load new values
        mov     ax,04F0Ch
        mov     [ebp].Client_Word_Reg_Struc.Client_AX,ax
        xor     ax,ax
        mov     [ebp].Client_Word_Reg_Struc.Client_BX,ax
        mov     ax,[esi].CRTCInfoBlock.PhysDotClock
        mov     [ebp].Client_Word_Reg_Struc.Client_CX,ax

        ; do the setmode
        mov     eax,010h
        VMMCall Exec_Int

        mov     dx,[ebp].Client_Word_Reg_Struc.Client_CX
        mov     cx,[ebp].Client_Word_Reg_Struc.Client_AX
        VMMCall End_Nest_Exec
        Pop_Client_State

        cmp     cx,0004Fh               ; did we do the clock call?
        jne     VESASupportPassDown     ; no, pass it down
ENDIF ; OLD_VESA30_REFRESH
        ;
        Push_Client_State
        VMMcall Begin_Nest_V86_Exec

        ; load new values
        mov     ax,[ebp].Client_Word_Reg_Struc.Client_SP
        sub     ax,((size CRTCInfoBlock + 1)/ 2) * 2
        mov     [ebp].Client_Word_Reg_Struc.Client_SP,ax
        mov     [ebp].Client_Word_Reg_Struc.Client_DI,ax
        mov     ax,[ebp].Client_Reg_Struc.Client_SS
        mov     [ebp].Client_Word_Reg_Struc.Client_ES,ax
        mov     ax,[ebp].Client_Word_Reg_Struc.Client_BX
        or      ax,00800h               ;set refresh rate flag
        mov     [ebp].Client_Word_Reg_Struc.Client_BX,ax

        ; copy block to VM stack
        Client_Ptr_Flat eax, SS, SP, USES_EAX

        .if (eax == -1)         ; -1 if error
                mov     ax, FALSE
        .else
                ;
                push    edx
                cld
                VMMcall _lmemcpy, <eax, esi, size CRTCInfoBlock>
                pop     edx

                ; Replace dot clock with the closest one found in the BIOS
                mov     edi,eax
                mov     [edi].CRTCInfoBlock.PhysDotClock,dx

                mov     InRefreshSetMode,0FFh

                ; do the setmode
                mov     eax,010h
                VMMCall Exec_Int
                ;push    10h
                ;VMMCall Exec_VxD_Int

                mov     InRefreshSetMode,000h

                mov     ax,TRUE                 ; did we do the setmode?
        .endif

        mov     cx,[ebp].Client_Word_Reg_Struc.Client_AX
        VMMCall End_Nest_Exec
        Pop_Client_State
        mov     [ebp].Client_Word_Reg_Struc.Client_AX,cx

        cmp     ax,TRUE                 ; did we do the setmode?
        jne     VESASupportPassDown     ; no, pass it down
;
VESASupportComplete:
        stc                             ; We completely handled the call
        mov     ax,VESA_SUCCESS         ; Return success
        jmp     VESASupportExit

ENDIF ; INCLUDE_REFRESH
        ;
        ; Hook out all VBE/DDC calls.  If we are currently running on a flat panel
        ; through the second I2C port, force the CX to 1.  Since the BIOS supports
        ; DDC3 it will accept the Monitor port # parameter (CX).
        ;
ifdef VESA_DDC
VESADDC:
        cmp     DontRecurse, 0          ;if we did an exec_int 10 4f15, we'll come right back here
        jne     VESASupportPassDown

;       Win98 does not recognize devices at address > A0.
;       It also does not recognize EDID 2.0.
;       We'll hack around these limitations.

        call    GetCRTCPort             ;EDX --> CRTC index register
        in      al,dx                   ;get and save current CRTC index ...
        ror     eax,8                   ;in high byte of EAX

;        unlock_extension_regs
        call    unlock_ext
;
        mov     al, 33h                 ;set to CRTC index 033h
        out     dx,al                   ;
        inc     edx                     ;EDX --> CRTC data register
        in      al,dx                   ;get data from CRTC register 033h
        dec     edx                     ;EDX --> CRTC index register

        test    al, 02h                 ;Check bit1, the current fp/crt bit
        jz      VESAMonitorActive       ;If 1, we are running a fp

;       if we are on DFP, what DDC port is it using?
        cmp             fpDDC,0
        je              VESAMonitorActive

        mov     [ebp].Client_Word_Reg_Struc.Client_CX, 1 ; Force to Monitor Port 1

VESAMonitorActive:
        call    restore_lock

;Restore the CRTC index register saved in the high byte of EAX:
;
        rol     eax,8                   ;
        out     dx,al                   ;

if VESA20   ; This code intercepts the VESA DDC calls to handle EDID 2.0 
            ; and device addresses other than A0, two cases which Win98 doesn't handle.
            ; We'll have to see if Millennium does this correctly, and bypass this if so.

        mov     bx,[ebp].Client_Word_Reg_Struc.Client_BX
        cmp     bl,0
        jz      VESAGetCaps
        cmp     bl,1
        jz      VESAReadEDID
        jmp     VESASupportPassDown
                
VESAGetCaps:
; Call into the BIOS, and check the return. If the device uses address > A0,
; change the return value to indicate A0, and set a flag to indicate we are tricking
; the OS and we should intercept the EDID read to get the correct device.
; No, we will always read the EDID, because we can't know if it is a version 2.
        mov     DontRecurse, 0ffh       ; we'll come right back to VESADDCSupport, pass it down
        
        Push_Client_State
        VMMcall Begin_Nest_V86_Exec
        mov     eax,010h
        VMMCall Exec_Int
        VMMCall End_Nest_Exec

        mov     DontRecurse, 0
        mov     DDCAddrFake,0          
        mov     bx,[ebp].Client_Word_Reg_Struc.Client_BX ; check the device address
        Pop_Client_State
        mov     [ebp].Client_Word_Reg_Struc.Client_BX,bx
        mov     DDCAddrFake,bx          ; save the DDC address map
        mov     [ebp].Client_Word_Reg_Struc.Client_AX,04fh

        test    bx,DDC_ADDR_A0          ; address A0 is OK, A2 & A6 Win98 can't handle
        jnz     VESASupportDone
        mov     bl,2                    ; make address A0
        mov     [ebp].Client_Word_Reg_Struc.Client_BX,bx
        jmp     VESASupportDone
        
VESAReadEDID:
; If we changed the previous GetCaps call to look like it was address A0 when it was actually 
; A2, we need to change the call to subfunction 3 -- Read EDID at A2.
; Because the monitor may have a version 2.0 EDID, we must always do the EDID read ourselves.
;        cmp     DDCAddrFake,0           ; are we tricking Windows?
;        jz      VESASupportPassDown

; save the caller's buffer pointer (possibly only 128 byte buffer)
        mov     ax,[ebp].Client_Word_Reg_Struc.Client_DI
        mov     BuffOff,ax
        mov     ax,[ebp].Client_Word_Reg_Struc.Client_ES
        mov     BuffSeg,ax

; alloc a 256 byte V86 buffer 
        VMMcall Get_Cur_VM_Handle       ; get the current vm handle into ebx
        mov     ecx, EDID2_SIZE         ; EDID 2 buffer size
        clc                             ; set carry flag to copy, clear otherwise
        VxDcall V86MMGR_Allocate_Buffer
        mov     eax,edi                 ; edi = (16 bits of seg, 16 bits of off)
        xor     ax,ax
        shr     eax,12
        or      ax,di
        mov     pEdid2,eax
                                        
        cmp     ecx, EDID2_SIZE
        jz      @f
        mov     ax,4f15h
        jmp     VESASupportPassDown

@@:     mov     eax, edi                ; Break the v86 address into ax:di
        and     edi, 0FFFFh
        shr     eax, 16

        mov     TempEdidBufOff,di
        mov     TempEdidBufSeg,ax
        
; Call into the BIOS, and check the return

; Set the device address, based on what we got from the Get DDC Caps call
; What is the proper address to use in the case of multiple addresses?
; A 256 byte device may indicate it supports A0, but the call will fail, since it returns
; the first 128 bytes, and the block will not checksum. (Seen on Gateway flat panel.)
; A 128 byte device will never indicate it supports A2 or A6.
; Therefore, if the device indicates it supports A2 or A6, use one of these, not A0.
        mov     EdidAddress, DDC_SUBF_4         ; read at A6
        test    DDCAddrFake,DDC_ADDR_A6
        jnz     DDC_Read_Exec_Int               ; use A6
EdidA2:        
        mov     EdidAddress, DDC_SUBF_3         ; read at A2
        test    DDCAddrFake,DDC_ADDR_A2
        jnz     DDC_Read_Exec_Int               ; use A2
EdidA0:        
        mov     EdidAddress, DDC_SUBF_1         ; read at A0
        
DDC_Read_Exec_Int:
        Push_Client_State                       ; preserve original state while we alter client register struct
; Insert our buffer pointer into the client state
        mov     ax,TempEdidBufOff
        mov     [ebp].Client_Word_Reg_Struc.Client_DI, ax
        mov     ax,TempEdidBufSeg
        mov     [ebp].Client_Word_Reg_Struc.Client_ES, ax
       
        mov     al,EdidAddress
        mov     [ebp].Client_Word_Reg_Struc.Client_BL,al
        mov     DontRecurse, 0ffh
        mov     [ebp].Client_Word_Reg_Struc.Client_AX, 4f15h
        
        VMMcall Begin_Nest_V86_Exec
        
        mov     eax,010h
        VMMCall Exec_Int
        VMMCall End_Nest_Exec
        mov     DontRecurse, 0
        mov     ax,[ebp].Client_Word_Reg_Struc.Client_AX        ; get return
        Pop_Client_State                        ; restore original state

        mov     [ebp].Client_Word_Reg_Struc.Client_AX, ax
        cmp     ax,04fh                                         ; put back return val
        jnz     Next_Edid_Addr

; Verify the function really succeeded
; In some cases, the monitor will respond to an address (ACK) but not return an EDID. The BIOS
; will then return Caps which are not true, and also report the EDID read succeeded when it did not.
; We will test the header data, and if it's not valid, retry using the next available address.
        mov     esi,pEdid2
        lea     edi,EDID1_HEADER
        mov     ecx,8
        repe    cmpsb           ; EDID 1?
        jz      Xlat_Edid_Buffer
        mov     esi,pEdid2
        cmp     byte ptr [esi],2 ; EDID 2?
        jz      Xlat_Edid_Buffer

; Try next address
Next_Edid_Addr:        
        cmp     EdidAddress,DDC_SUBF_1
        je      EdidDone
        cmp     EdidAddress,DDC_SUBF_4
        je      EdidA2
        cmp     EdidAddress,DDC_SUBF_3
        je      EdidA0
        jmp     DDC_Read_Exec_Int
                 
Xlat_Edid_Buffer:
; Get flat address for the caller's buffer
        xor     eax,eax
        mov     ax,BuffSeg
        shl     eax,4
        xor     ecx,ecx
        mov     cx,BuffOff
        add     eax,ecx
        mov     pEdid1,eax
                
; If it was a version 2.0 EDID, make up a fake version 1.x
        mov     esi,pEdid2
        mov     al,[esi]
        cmp     al,20h      ; version 2.0?
        je      Edid2

; Copy version 1 EDID into caller's buffer
        mov     cx,EDID1_SIZE
        mov     edi,pEdid1
        mov     esi,pEdid2
        rep movsb                
        jmp     EdidDone    

; We have a version 2 EDID. Translate into version 1 and return to caller.
; We do the minimum work required to make the VDD happy.
; This includes the product/vendor ID and some detailed timings.
Edid2:
        ; copy template
        mov     edi,pEdid1
        mov     esi,offset32 EdidTemp
        mov     ecx,EDID1_SIZE
        rep movsb
        ; copy vendor/product ID
        mov     esi,pEdid2
        mov     edi,pEdid1
        mov     ax,[esi+EDID2_VENDOR]
        mov     [edi+EDID1_VENDOR],ax

        ; Copy detailed timings. We have to parse a bunch of fields to find them first.
        mov     ax,[esi+EDID2_MAP]      ; map of timing info tells where to start
        mov     Edid2Map,ax
        mov     ecx,EDID2_TIMING_START   
        test    al, EDID2_LUM_TABLE_EXIST ; any lum tables?
        jz      Edid_rl
        ; advance beyond lum table
        mov     ah,[esi+ecx]            ; get 1st byte of timing table
        mov     al,ah
        and     al,EDID2_LUM_ENTRIES    ; get # of lum tables entries
        test    ah,EDID2_LUMX3          ; triple lum tables?
        jz      @f
        mov     bl,3
        mul     bl                      ; triple entries
@@:     add     al,1                    ; bump index beyond lum table
        add     cl,al                
Edid_rl:
        mov     ax,Edid2Map
        and     al,EDID2_RL             ; any range limits?
        jz      Edid_drl
        ; advance beyond range limits        
        shr     al,EDID2_RL_SHIFT
        mov     bl,8
        mul     bl                      ; 8-byte range limits
        add     cl,al
Edid_drl:
        mov     ax,Edid2Map
        test    al,EDID2_DRL            ; any detailed range limits?
        jz      Edid_tc
        ; advance beyond detailed range limits
        and     al,EDID2_DRL
        mov     bl,27
        mul     bl                      ; 27-byte detailed range limits
        add     cl,al
Edid_tc:
        mov     ax,Edid2Map             ; get 2nd byte of map
        and     ah,EDID2_TIMING_CODES
        jz      Edid2_dt
        shr     ah,EDID2_TC_SHIFT
        mov     al,ah
        mov     bl,4
        mul     bl                      ; 4-byte timing codes
        add     cl,al
        ; now we are finally at the detailed timings
Edid2_dt:
        mov     ax,Edid2Map
        and     ah,EDID2_DET_TIMINGS
        mov     al,ah
        mov     bl,18
        mul     bl                      ; 18-byte detailed timings
        mov     nDetTim,al
        mov     DetTimOff,cx            ; offset within edid2 struct to detailed timings
        ; copy all detailed timings to edid1
        add     edi,EDID1_DET_TIMING    ; point to edid1 detailed timings
        add     esi,ecx                 ; point to edid2 detailed timings
        xor     ecx,ecx
        mov     cl,nDetTim
        rep movsb
        
        ; generate 8 bit checksum
        mov     ecx,EDID1_SIZE
        mov     edi,pEdid1
        xor     ax,ax
EdidCs: add     al,[edi+ecx-1]
        loop    EdidCs
        neg     al
        mov     [edi+EDID1_SIZE-1],al
;debug - fail the call to see if the system hangs
;        mov     [ebp].Client_Word_Reg_Struc.Client_AX,014fh

EdidDone:
; Free the V86 buffer
        VMMcall Get_Cur_VM_Handle       ; get the current vm handle into ebx
        mov     ecx,EDID2_SIZE
        clc                             ; no copy back required
        VxDcall V86MMGR_Free_Buffer
        jmp     VESASupportDone
endif ; VESA_DDC

VESASupportDone:
        stc                             ; we handled it
        jmp     VESASupportExit

endif ; VESA20

VESASupportPassDown:
        clc                             ; We didn't completely handle the call, pass down to BIOS

VESASupportExit:
        popad
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret
EndProc MiniVDD_VESASupport
;
;


;       Hook Int 10
;       Check for VESA DDC function and direct it to the appropriate controller
BeginProc    MiniVDD_Int10Handler
        pushad
      
        mov     ax,[ebp].Client_Word_Reg_Struc.Client_AX
        
        ; Don't let VGA.DRV on secondary do a modeset when our driver is running on primary
        cmp     NumUnits,2              ; TwinView?
        jne     @f                      ; no, act normally
        cmp     DisplayEnabledFlag,0    ; display driver up?
        jz      @f                      ; no, it's not a problem
        cmp     DosBoxActive,0          ; running FS DOS?
        jne     @f                      ; yes, let it go thru
        cmp     ax,12h                  ; VGA driver modeset?
        jnz     @f                      ; yes, this is what we want to prevent

        ; We should skip modeset call from Windows VM only. This is workaround of Windows bug
        ; when VGA display driver on the secondary device (Display settings tab multimon) calls
        ; INT10 because it thinks we are in VGA mode but it does not assume it executes
        ; on the secondary device. VBIOS INT10 always executes on primary device so we'll change
        ; timings on primary by mistake.
        VMMCall Get_Cur_VM_Handle
        push    ebx
        VMMCall Get_Sys_VM_Handle
        pop     edx
        cmp     ebx,edx                 ; Bug #33723
        jne     @F                      ; Do not skip modeset if not Windows WM

        ; This may be a problem for Asian language DOS, which uses mode 12.
        ; Does a virtualized DOS session do a mode 12, and if we swallow it, what happens?
        jmp     Int10_Handled            ; Then swallow the interrupt

@@:
        ; Check for DDC functions
        cmp     ax,4F15h                ; Is this a VBE/DDC call?
        jne     Int10_Not_Handled

        ; We'll let the BIOS handle the report DDC caps call (BL = 0),
        ; and we'll handle the read EDID (BL = 1),

ifndef RM_GET_DDC_CAPS
        ; if we let the RM handle GET_DDC_CAPS, we don't need to do this
        ; Make sure that CRTC "A" owns I2C, since that is the head the BIOS will use to do DDC.
        ; unlock_extension_regs
        call    unlock_ext
        mov     dx,3d4h
        mov     ax,044h                 ; point to head A
        out     dx,ax
        mov     al,49h                  ; Engine Control reg
        out     dx,al
        inc     dx
        in      al,dx
        or      al,04h                 ; head A claims I2C (takes precedence over head B claiming it, so no need to set the other CR49)
        out     dx,al
        call    restore_lock
endif

        mov     bx,[ebp].Client_Word_Reg_Struc.Client_BX

        cmp     bl,01h                  ; Read Edid ?
        je      VESA_Read_EDID

ifndef  RM_GET_DDC_CAPS
        jmp     Int10_Not_Handled       ; let BIOS do it
else
        cmp     bl, 0
        jne     Int10_Not_Handled
                                        ; let RM do it
        mov     eax, NVRM_API_GET_DDC_CAPS
        mov     ebx, DEFAULT_PRIMARY_HDEV ; primary device
        call    CallRM                  ; returns status in ax
        mov     [ebp].Client_Word_Reg_Struc.Client_BX, bx
        mov     [ebp].Client_Word_Reg_Struc.Client_AX, 4fh
        jmp     Int10_Handled
endif

VESA_Read_EDID:
        ; This is a trick. Windows enabled EDID PNP support
        ; if it read EDID by the VESA int10 call AND it
        ; receives a valid EDID. Just let it go through
        ; here until it gets one good EDID and then we will
        ; stop punting to here from GetMonitorInfo.

        ; If GetMonInfoCount < 0 then that is either SBIOS call
        ; or that some DOS app is issuing the VESA call
        ; and not the main VDD I suppose.
        cmp     GetMonInfoCount,0
        js      Int10_Not_Handled

        ; Get flat address for the caller's buffer
        xor     edi,edi
        mov     di,[ebp].Client_Word_Reg_Struc.Client_ES
        shl     edi,4
        xor     eax,eax
        mov     ax,[ebp].Client_Word_Reg_Struc.Client_DI
        add     edi,eax

        ; ecx = Head number
        inc     dwInt10EntryCount
        jne     Int10_Entry_Count_Not_Handled

        push    edi
        push    GetMonInfoCount
        push    dwDisplayDevnodeHandle
        call    GetEdidAndRemapIt
        add     esp,0CH

        dec     dwInt10EntryCount

        ; Assume that there was an EDID
        mov     GetMonInfoCount,-2
        mov     eax,[edi]
        or      eax,eax
        jne     @F

        ; Reset GetMonInfoCount because it might be non DDC
        ; monitor on the head 0. Windows will not enable EDID
        ; PNP support unless it uses the int10 call AND it
        ; receives a valid EDID.
        mov     GetMonInfoCount,-1

        ; if fail, return VESA DDC fail status
        mov     [ebp].Client_Word_Reg_Struc.Client_AX,014Fh
        jmp     Int10_Handled

@@:
        mov     [ebp].Client_Word_Reg_Struc.Client_AX,004Fh

Int10_Handled:
        clc                            ; clear carry to show we handled it
        jmp     Int10_Done

Int10_Entry_Count_Not_Handled:
        dec     dwInt10EntryCount      ; fallthrough to stc
Int10_Not_Handled:
        stc                            ; set carry to show we didn't handle it
Int10_Done:
        popad
        ret
EndProc MiniVDD_Int10Handler


public  SaveNVAccessState
BeginProc SaveNVAccessState
;
;       SaveNVAccessState - Save the state of the NV real mode access registers
;
;       Entry:  Extended CRTC registers unlocked
;       Exit:   esi = 32-bit NV address
;               al  = real mode access control bits
;               dx  = LSW of 32-bit data register
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "SaveNVAccessState", DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        push    ecx
        push    eax
        push    edx
        ;
        call    GetCRTCPort
        mov     al, NVRM_ACCESS
        SIM_NO  dx,al
        inc     edx                                     ; edx -> CRTC data
        SIM_NI  al,dx
        ror     eax,8                                   ; Save access bits in high byte of eax
        dec     edx                                     ; edx -> CRTC index
        mov     al, NVRM_ACCESS
        mov     ah, NVRM_ACCESS_MASK OR NVRM_DATA32     ; Get LSW of 32-bit write
        SIM_NO  dx, ax
        ror     edx,16                                  ; save CRTC index in upper word of edx
        mov     dx,003D0h
        SIM_NI  ax,dx
        push    ax                                      ; Save LSW of 32-bit write
        mov     al, NVRM_ACCESS
        mov     ah, NVRM_ACCESS_MASK OR NVRM_ADDRESS
        rol     edx,16                                  ; restore CRTC index
        SIM_NO  dx, ax
        ror     edx,16                                  ; save CRTC index in upper word of edx
        mov     dx, 003D0h
        SIM_NI  ax,dx
        push    ax                                      ; Save LSW of address
        inc     dx
        inc     dx
        SIM_NI  ax,dx
        mov     si,ax                                   ; move MSW of address into si
        shl     esi, 16
        pop     si                                      ; now ESI has whole address
        rol     edx,16                                  ; restore CRTC index
        rol     eax,8                                   ; restore access bits
        inc     edx                                     ; DX -> CRTC data register
        SIM_NO  dx,al
        ;
        pop     cx                                      ; restore LSW of 32-bit write
        pop     edx                                     ; restore original edx
        mov     dx,cx                                   ; now edx is all set
        ;
        mov     cl,al                                   ; save control bits into cl
        pop     eax                                     ; restore original eax
        mov     al,cl                                   ; now eax is all set
        ;
        pop     ecx                                     ; restore original ecx
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret
EndProc SaveNVAccessState
;
;
public  RestoreNVAccessState
BeginProc RestoreNVAccessState
;
;       RestoreNVAccessState - Restore the state of the NV real mode access registers
;
;       Entry:  Extended CRTC registers unlocked
;               esi = 32-bit NV address
;               al  = real mode access control bits
;               dx  = LSW of 32-bit data register
;       Exit:   none
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf "RestoreNVAccessState", DEBLEVELMAX
                push    ecx
                and     ecx,0FFFFh
                Debug_Printf ":CRTC:%04X", ecx, DEBLEVELMAX
                pop     ecx
                pop     eax
                ;
                push    eax
                Debug_Printf ":NV Address:%08lX:RM Control:%04X:RM Reg:%04X:LSW Data32:%04X", eax, DEBLEVELMAX
                push    ebx
                mov     bl,bh
                and     ebx,0FFh
                Debug_Printf ":RM Control:%04X:RM Reg:%04X:LSW Data32:%04X", ebx, DEBLEVELMAX
                pop     ebx
                push    ebx
                and     ebx,0FFh
                Debug_Printf ":RM Reg:%04X:LSW Data32:%04X", ebx, DEBLEVELMAX
                pop     ebx
                push    edx
                and     edx,0FFFFh
                Debug_Printf ":LSW Data32:%04X", edx, DEBLEVELMAX
                pop     edx
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        push    eax
        push    edx
        ;
        push    ax                                      ; we need to use these
        push    dx
        ;
        call    GetCRTCPort
        mov     al, NVRM_ACCESS
        mov     ah, NVRM_ACCESS_MASK OR NVRM_ADDRESS
        SIM_NO  dx, ax
        ror     edx,16                                  ; save CRTC index in upper word of edx
        mov     dx, 003D2h                              ; Write MSW of address first
        mov     eax, esi
        shr     eax, 16
        SIM_NO  dx, ax
        dec     dx                                      ; Now write LSW of address
        dec     dx
        mov     ax, si
        SIM_NO  dx, ax
        mov     al, NVRM_ACCESS
        mov     ah, NVRM_ACCESS_MASK OR NVRM_DATA32     ; LSW of 32-bit write
        rol     edx,16                                  ; restore CRTC index
        SIM_NO  dx, ax
        ror     edx,16                                  ; save CRTC index in upper word of edx
        mov     dx,003D0h
        pop     ax                                      ; Restore LSW of 32-bit write
        SIM_NO  dx, ax
        rol     edx,16                                  ; restore CRTC index
        pop     ax                                      ; Restore access bits
        mov     ah, al
        mov     al, NVRM_ACCESS
        SIM_NO  dx, ax
        ;
        pop     edx
        pop     eax
IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret
EndProc RestoreNVAccessState
;
;
IFDEF DOS_REFRESH
subttl          Handle refresh rate support for PM OEM VESA calls
page +
public MiniVDD_Xlat_Int10
BeginProc MiniVDD_Xlat_Int10
;BeginProc MiniVDD_Xlat_Int10, VMCREATE
;
;
;* need to map pm to real mode int 10, 4f14,41
;
IFDEF DEBUG_LOG
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "MiniVDD_PMHandler", DEBLEVELMAX
                pop     eax
        .endif
ENDIF ; DEBUG_LOG
        VMMcall Simulate_Iret                   ; Eat the int right now

        mov     eax, [ebp.Client_EAX]           ; Get entry EAX

        cmp     ax, 4F14h                       ; Q: Get/Set palette?
        jne     SHORT MVXI10_Reflect_Int_EDX    ;    N: Just reflect it now

        mov     ax,[ebp].Client_Word_Reg_Struc.Client_BX
        cmp     al,041h                         ; OEM set default refresh rate
        jne     SHORT MVXI10_Reflect_Int_EDX    ;    N: Just reflect it now

        mov     edx, OFFSET32 MiniVDD_OEM_Refresh_API
        VxDjmp  V86MMGR_Xlat_API

public MVXI10_Reflect_Int_EDX
MVXI10_Reflect_Int_EDX:
        mov     edx, OFFSET32 MiniVDD_Default_API
        VxDjmp  V86MMGR_Xlat_API

IFDEF DEBUG_LOG
        pushf
        .if (DebugOn != 0)
                push    eax
                Debug_Printf    "\n\r", DEBLEVELMAX
                pop     eax
        .endif
        popf
ENDIF ; DEBUG_LOG
        ret
EndProc MiniVDD_Xlat_Int10
ENDIF ; DOS_REFRESH

IFDEF   I2C
;       I2COpen
;
;Entry: args on stack (stdcall)
;
;Exit:  status
public  MiniVDD_I2COpen
BeginProc       MiniVDD_I2COpen, DOSVM
;       args are passed to us on the stack as stdcall
        call    _vddI2COpen
        ret 12
EndProc         MiniVDD_I2COpen

;       I2CAccess
;
;Entry: args on stack (stdcall)
;
;Exit:  status
;
public  MiniVDD_I2CAccess
BeginProc       MiniVDD_I2CAccess, DOSVM
;       args are passed to us on the stack as stdcall
        call    _vddI2cAccess
        ret 8
EndProc         MiniVDD_I2CAccess

ENDIF   ;I2C

IFDEF   DIOC
subttl      WIN32 Device IO Control
page +
public  MiniVDD_W32_DIOC

BeginProc MiniVDD_W32_DIOC
;-----------------------------------------------------------------
;       D E V I C E I O C O N T R O L   I N T E R F A C E
;-----------------------------------------------------------------
;  ENTRY :
;        ESI -> address of DIOC structure

        pushad
        mov  ecx, [ esi ].dwIoControlCode

        cmp  ecx,20000700H
        je   DevCtl_SetFSDOSPtr

        cmp  ecx, DIOC_CLOSEHANDLE
        je   DevCtl_CloseHandle

        cmp  ecx, DIOC_GETVERSION
        je   DevCtl_GetVersion

        cmp  ecx, VDD_IOCTL_NV_GETSETCALLBACK
        jnz  Next0
        call NVGetSetCallback
        jmp  MiniVDDDone

Next0:
        cmp  ecx, VDD_IOCTL_NV_FILLTABLE
        jnz  Next1
        call NvFillTable
        jmp  MiniVDDDone
Next1:
        cmp  ecx, VDD_IOCTL_GET_DDHAL
        jnz  Next2
        call GetDDHAL
        jmp  MiniVDDDone
Next2:
        cmp  ecx, VDD_IOCTL_NV_SETVIDTEXSURF
        jnz  Next3
        call NVSetVidTexSuf
        jmp  MiniVDDDone
Next3:
IFDEF NVPE
        cmp  ecx, VDD_IOCTL_NVPE_COMMAND
        jnz  MiniVDDDone
        push esi
        call _nvpeVDDCmdHandler
        pop  esi
ENDIF

MiniVDDDone:
        cmp  eax,1
        jae  DevCtl_Fails

;----------------------------------------------------------------
;
;  ECX = -1 ( DIOC_CLOSEHANDLE )
;
;----------------------------------------------------------------

DevCtl_CloseHandle      :

        jmp  DevCtl_OKey

DevCtl_SetFSDOSPtr:
        mov  eax,[esi].lpvInBuffer
        mov  pDDrawFullScreenDOSActive,eax
        jmp  DevCtl_OKey

;----------------------------------------------------------------
;
;  ECX = 0 ( DIOC_GETVERSION )
;
;----------------------------------------------------------------
DevCtl_GetVersion :

        jmp  DevCtl_OKey

DevCtl_Fails :

        mov DevCtlRet, 50    ; ERROR_NOT_SUPPORTED
        stc                  ; indicate error
        jmp DevCtl_Exit

DevCtl_OKey :

        mov DevCtlRet, 0
        clc
        jmp DevCtl_Exit

DevCtl_Exit :

        popad
        mov eax, DevCtlRet
        ret
EndProc MiniVDD_W32_DIOC
endif   ;DIOC


public  WaitTicks
BeginProc WaitTicks
;
;       WaitTicks - Wait a given number of timer ticks
;
;       Entry:  CX = Number of ticks to wait
;       Exit:   None
;
;       All registers are preserved
;
        pushad
;
        VMMCall Get_System_Time
        mov     ebx,eax
@@:
        VMMCall Get_System_Time
        sub     eax,ebx
        cmp     ax,cx
        jl      @b
;
        popad
        ret
EndProc WaitTicks
;
;
public  Beep
BeginProc Beep
;
;       Beep - Beep the speaker once
;
;       Entry:  None
;       Exit:   None
;
;       All registers are preserved.
;
;
IFDEF   DEBUG_BEEP
        pushad
;
        mov     al,0B6h
        out     43h,al
        mov     al,033h
        out     42h,al
        mov     al,005h
        out     42h,al
        in      al,061h
        push    ax
        or      al,003h
        out     61h,al
        mov     cx,4*18
        call    WaitTicks
        pop     ax
        out     61h,al
;
        popad
ENDIF   ; DEBUG_BEEP
        ret
EndProc Beep
;
;
; Call the resource manager.
;

BeginProc       CALLRM          
;               push everything onto stack so we have the top of a client reg struct. (see VMM.H)

        push    esi
        mov     esi, offset callrm_rmusrapi
        mov     dword ptr [esi].minivdd_CALLRM_rmusrapi_Struct.rmusrapi_function, eax
        dec     ebx
        mov     dword ptr [esi].minivdd_CALLRM_rmusrapi_Struct.rmusrapi_device, ebx
        mov     eax,esi
        pop     esi 

        
        push    eax
        push    ecx
        push    edx
        push    ebx
        push    eax                             ; res0, documented as "ESP at pushall"
        push    ebp
        push    esi
        push    edi
        
        mov     ebp, esp                ; point ebp to client reg struct 
        call    DWORD PTR ResManAPI
        pop     edi
        pop     esi
        pop             ebp
        pop             eax                             ; res0
        pop     ebx
        pop     edx
        pop     ecx
        pop     eax
        ret     
EndProc CALLRM

;               save lock value and unlock chip
;               we must save lock value. CRTC index should be saved in calling routine
BeginProc       unlock_ext
        push    eax
        push    edx
        call    GetCRTCPort             ;EDX --> CRTC index register
                mov             al,1fh
                out             dx,al
                inc             dx
                in              al,dx
                mov             LockValue,al
                mov             al,57h
                out             dx,al
sl_exit:
        pop     edx
        pop     eax
        ret
                                
EndProc unlock_ext






;               restore lock state
BeginProc       restore_lock
        push    eax
        push    edx
        call    GetCRTCPort             ;EDX --> CRTC index register
;       NV4 - locked, unlocked, or read-only
                mov             ah,57h
                cmp             LockValue,3             ;read 1 if unlocked
                je              @f
                mov             ah,75h
                cmp             LockValue,1             ;read 1 if read-only (NV4 or later)
                je              @f
                mov             ah,99h                  ;must be locked
@@:         ;NV4
                mov             al,1fh
                out             dx,ax                   ;restore lock
rl_exit:
        pop     edx
        pop     eax
        ret
EndProc         restore_lock


;; ReadCr44 - BIOS copied script for reading CR44
;; INPUTS
;;    DX - CRTC access (3d4)
;; OUTPUT
;;    AL - Cr44 read value
;;    DX - restored
BeginProc       ReadCr44
        ; TODO: We probably should read the Resman copy
        ; since I'm not sure if this io read works
        ;
        ; Read Cr44 using 8 bit IO - Output will be in AL
        mov     al,44h          ; Address CR44/0 AH for default
        out     dx,al           ; send 44 as index
        inc     edx             ; set to 3d5
        in      al,dx
        dec     edx             ; set back to 3d4
        
        ; Read the mirror bit. If this is on, we know we're in Broadcast mode
;        push    ebx
;        xchg    eax,ebx         ; store eax in ebx for now
        ; get the mirror bit (28)
;        REG_RD32(NV_PBUS_DEBUG_1)
;        xchg    eax,ebx         ; restore eax, PBUS_DEBUG_1 is in ebx
        ; is the mirror bit set
;        .if (ebx & 10000000h)
;                mov     al,04h  ; return Broadcast mode
;        .endif
;        pop     ebx
        
        ret
EndProc         ReadCr44
        
;;
;; WriteCr44 - BIOS copied script for writing CR44
;; INPUTS
;;    DX - CRTC access (3d4)
;;    AL - Cr44 write value value
;; OUTPUT
;;    DX - restored
BeginProc       WriteCr44
        ; Using MMIO to write CR44
        ; This works for the Resman!
        push    eax
        mov     al,44h
        REG_WR08(NV_PRMCIO_CRX__COLOR)
        pop     eax
        REG_WR08(NV_PRMCIO_CRE__COLOR)

        ret
EndProc         WriteCr44





; Procedures for setting BIOS TV programming flag
; Use memory mapped access because we can assure access to head A on multihead devices, without having to switch CR44.
BeginProc TellBIOSProgramTV
;   tell the BIOS to program the TV encoder
        push    eax
        push    edx
        push    ecx
        push    esi                     ; used in REG_RD/REG_WR macros

            ; unlock
        mov al, 1Fh                     ; Get current CR1F
        REG_WR08(NV_PRMCIO_CRX__COLOR)
        REG_RD08(NV_PRMCIO_CRE__COLOR)  ; Save it
        mov cl, al
        mov al,57h
        REG_WR08(NV_PRMCIO_CRE__COLOR)  ; unlock

        mov al, BIOS_SCRATCH1
        REG_WR08(NV_PRMCIO_CRX__COLOR)  
        REG_RD08(NV_PRMCIO_CRE__COLOR)  
        and al,NOT BIOS_PGRM_TV         ; clear bit 1 (OK to program TV encoder)
        REG_WR08(NV_PRMCIO_CRE__COLOR)  

        mov al, 1Fh                     ; Get current CR1F
        REG_WR08(NV_PRMCIO_CRX__COLOR)  
        ; restore lock
        mov             al,57h
        cmp             cl,3             ;read 1 if unlocked
        je              @f
        mov             al,75h
        cmp             cl,1             ;read 1 if read-only (NV4 or later)
        je              @f
        mov             al,99h           ;must be locked
@@:     
        REG_WR08(NV_PRMCIO_CRE__COLOR)  

        pop     esi                     ; used in REG_RD/REG_WR macros
        pop     ecx
        pop     edx
        pop     eax
        ret
EndProc TellBIOSProgramTV


BeginProc TellBIOSNotProgramTV
;   tell the BIOS to not program the TV encoder
        push    eax
        push    edx
        push    ecx
        push    esi                     ; used in REG_RD/REG_WR macros

        ; unlock
        mov al, 1Fh                     ; Get current CR1F
        REG_WR08(NV_PRMCIO_CRX__COLOR)  
        REG_RD08(NV_PRMCIO_CRE__COLOR)  ; Save it
        mov cl, al
        mov al,57h
        REG_WR08(NV_PRMCIO_CRE__COLOR)  ; unlock

        mov al, BIOS_SCRATCH1
        REG_WR08(NV_PRMCIO_CRX__COLOR)  
        REG_RD08(NV_PRMCIO_CRE__COLOR)  
        or  al, BIOS_PGRM_TV            ; set bit 1 (not OK to program TV encoder)
        REG_WR08(NV_PRMCIO_CRE__COLOR)  

        mov al, 1Fh                     ; Get current CR1F
        REG_WR08(NV_PRMCIO_CRX__COLOR)  
        ; restore lock
        mov             al,57h
        cmp             cl,3             ;read 1 if unlocked
        je              @f
        mov             al,75h
        cmp             cl,1             ;read 1 if read-only (NV4 or later)
        je              @f
        mov             al,99h           ;must be locked
@@:     
        REG_WR08(NV_PRMCIO_CRE__COLOR)  
        
        pop     esi                     ; used in REG_RD/REG_WR macros
        pop     ecx
        pop     edx
        pop     eax
        ret
EndProc TellBIOSNotProgramTV
;
VxD_LOCKED_CODE_ENDS
;
;
end
