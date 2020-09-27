;       (C) Copyright NVIDIA Corporation Inc., 1996,1997. All rights reserved.
;       Portions Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.
;
;       NV3MINI2.ASM - NVidia secondary Mini-VDD for NV3
;
;       Written by:     JohnH
;       Date:           12/17/97
;
;
;
page            ,132
title           NVidia NV3 Secondary Mini-VDD Support Functions
.386p
;

;
.xlist
MINIVDD                 EQU             1               ; this must be defined for minivdd.inc

include         VMM.INC
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
include                  SURFACES.INC

IFDEF   DIOC
extrn           GetDDHAL:near
extrn                            NvFillTable:near
extern                   NVGetSetCallback:near
extern          NVSetVidTexSuf:near
ENDIF   ;DIOC

.list

;**************************************************************************************************************************
subttl          Virtual Device Declaration
page +
;       Then MiniVDD must have the UNDEFINED_DEVICE_ID
;       and it is a dynamic load VxD, so it cannot have a service table.
Declare_Virtual_Device  NVMINI2, \
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
NVRM_String            db      'NVCORE.VXD',0
VxD_IDATA_ENDS
;
;**************************************************************************************************************************
subttl          Locked Data Area
page +
VxD_LOCKED_DATA_SEG
TotalMemorySize dd              400000h
VxD_LOCKED_DATA_ENDS
;
;**************************************************************************************************************************
subttl          General Data Area
page +
VxD_DATA_SEG
;
OSVersion       dw      0
ResManAPI       dd      0
devNode         dd      0
nvAddr          dd      0
nvPhys          dd      0
fbPhys          dd      0
fbLength        dd      0
nvIRQ           dd      0
hDev            dd      0
DevCtlRet       dd      0
NumUnits	dd      0               ; number of "units" or heads
;
public  VGAModeFlag
VGAModeFlag             db      0               ;=FFh when in 16 color mode (VGA driver)


; JJV-MM
; This Minivdd must handle up to 8 secondary devices (7 if the primary is also an NV4).
minivdd_instance_Struct struct
  instance_devNode          dd      0
  instance_nvAddr           dd      0
  instance_nvPhys           dd      0
  instance_fbPhys           dd      0
  instance_fbLength         dd      0
  instance_nvIRQ            dd      0
  instance_hDev             dd      0
  instance_ACPI_state       dd      0
minivdd_instance_Struct ends
; Static structures for each instance.
device1_instance minivdd_instance_Struct <>
device2_instance minivdd_instance_Struct <>
device3_instance minivdd_instance_Struct <>
device4_instance minivdd_instance_Struct <>
device5_instance minivdd_instance_Struct <>
device6_instance minivdd_instance_Struct <>
device7_instance minivdd_instance_Struct <>
device8_instance minivdd_instance_Struct <>

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

public  DisplayInfoStructure
DisplayInfoStructure    DISPLAYINFO     <>

PUBLIC  dwCreatingVM
dwCreatingVM            DD      0

JIMK1   EQU     <1>

IF JIMK1
PUBLIC  dwDisplayDevnodeHandle
dwDisplayDevnodeHandle  DD      0

PUBLIC  szEnumAndMore
szEnumAndMore   DB      "Enum\"
szAndMore       DB      128 DUP(0)

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
ENDIF

VxD_DATA_ENDS
;
;
;**************************************************************************************************************************
subttl          Device Initialization
page +
VxD_ICODE_SEG

public  MiniVDD_Dynamic_Exit
BeginProc MiniVDD_Dynamic_Exit
   xor eax, eax
   ret
EndProc MiniVDD_Dynamic_Exit

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
;
;               Load the resource manager. If already loaded, this will just return the entry point.
                mov             edx, OFFSET32 NVRM_String
                mov             eax, VXDLDR_INIT_DEVICE
                VxDcall VXDLDR_LoadDevice
        jnc     @f
                jmp             MVDIErrorExit                   ;failed, nothing we can do

@@:
;       returns DDB address in eax and device handle in edx
;       save the API entry
                mov             eax, [eax].DDB_PM_API_Proc
                mov             ResManAPI, eax
;

;	Correct the string for multihead configuration
;	Change %s\Unit%d --> %s\U%d
	push	ecx
	push	edx
	push	edi
	mov	eax,0ah
	xor	edi,edi
	VMMCall	Get_DDB
	or	ecx,ecx
	jz	@F
	lea	edi,[ecx+18h]
	mov	edx,ASSERT_RANGE_NO_DEBUG OR ASSERT_RANGE_NULL_BAD
	VMMCall	_Assert_Range,<edi,4,0,0,edx>
	or	eax,eax
	jz	@F
	mov	edi,[edi]
	lea	edi,[edi+13fch]
	VMMCall	_Assert_Range,<edi,4,2574696eh,0,edx>
	or	eax,eax
	jz	@F
	mov	ecx,6425h		; '%d'
	mov	[edi],ecx
@@:
	pop	edi
	pop	edx
	pop	ecx
;
public  MVDIDispatch
MVDIDispatch:
;
;
;Fill in the addresses of all the functions that we need to handle in this
;mini-VDD in the table provided by the "master" VDD whose address is
;contained in EDI.  Note that if you do not need to support a function,
;you do not need to fill in the dispatch table entry for that function.
;If you do not fill in an entry, the "master" VDD won't try to call
;this mini-VDD to support the function.  It'll just handle it in a
;default manner.
;
        VxDCall VDD_Get_Mini_Dispatch_Table
        cmp     ecx,NBR_MINI_VDD_FUNCTIONS      ;perform a cursory version check
        jb      MVDIErrorExit                   ;oops, versions don't match!
;
public  MVDI_FillInTable
MVDI_FillInTable:
        MiniVDDDispatch REGISTER_DISPLAY_DRIVER,RegisterDisplayDriver
        MiniVDDDispatch GET_TOTAL_VRAM_SIZE,GetTotalVRAMSize
        ;
        ; new Win98 functions...
        ;
        VMMcall Get_VMM_Version
;        .if (!(carry?) && (ax == 040ah))
        jc  PreWin98
        mov   OSVersion,ax
        cmp   ax,VERS_WIN98
        jb    PreWin98
;       The primary needs to support these
        MiniVDDDispatch TURN_VGA_OFF,TurnVgaOff
        MiniVDDDispatch TURN_VGA_ON,TurnVgaOn

;       This is required if the device supports multiple displays (NV4?)
        MiniVDDDispatch GET_NUM_UNITS,GetNumUnits

         ;These are for power management. We don't need the monitor functions, since Windows will use VESA calls
         ; if we don't hook them.
;ifdef ON_NOW
          MiniVDDDispatch SET_ADAPTER_POWER_STATE,SetAdapterPowerState
          MiniVDDDispatch GET_ADAPTER_POWER_STATE_CAPS,GetAdapterPowerStateCaps
;endif
;          MiniVDDDispatch SET_MONITOR_POWER_STATE,SetMonitorPowerState
;          MiniVDDDispatch GET_MONITOR_POWER_STATE_CAPS,GetMonitorPowerStateCaps
;        .endif
PreWin98:

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
                Control_Dispatch SYS_VM_TERMINATE, MiniVDD_Sys_VM_Terminate             ;try to release traps here
                ;Control_Dispatch SYS_DYNAMIC_DEVICE_EXIT, MiniVDD_Dynamic_Exit  ;try to release traps here
            Control_Dispatch W32_DEVICEIOCONTROL, MiniVDD_W32_DIOC
        ;
End_Control_Dispatch MiniVDD

public  MiniVDD_W32_DIOC
BeginProc MiniVDD_W32_DIOC
;-----------------------------------------------------------------
;       D E V I C E I O C O N T R O L   I N T E R F A C E
;-----------------------------------------------------------------
;  ENTRY :
;        ESI -> address of DIOC structure

        pushad

        mov  ecx, [ esi ].dwIoControlCode
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
        call   NvFillTable
        jmp     MiniVDDDone
Next1:
        cmp  ecx, VDD_IOCTL_GET_DDHAL
        jnz  Next2
        call GetDDHAL
        jmp  MiniVDDDone
Next2:
   cmp  ecx, VDD_IOCTL_NV_SETVIDTEXSURF
        jnz  MiniVDDDone
        call NVSetVidTexSuf


MiniVDDDone:
        cmp     eax,1
        jae  DevCtl_Fails

;----------------------------------------------------------------
;
;  ECX = -1 ( DIOC_CLOSEHANDLE )
;
;----------------------------------------------------------------

DevCtl_CloseHandle      :

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

;
;--------------------------------------------------------------------------
public  MiniVDD_TurnVgaOff
BeginProc       MiniVDD_TurnVgaOff, DOSVM

                push    eax
                push    esi

                ;Disable VGA through the VGA Enable IO register.
                REG_RD08(NV_PRMVIO_VSE2)
                and al,0feh
                REG_WR08(NV_PRMVIO_VSE2)

                ;Disable NV PCI IO, bit0 = 0.
                REG_RD32(NV_PBUS_PCI_NV_1)
                and     al,0feh
                REG_WR32(NV_PBUS_PCI_NV_1)

                ;Disable NV interrupts.
;;                xor     eax,eax
;;                REG_WR32(NV_PMC_INTR_EN_0)

                pop     esi
                pop     eax

                ret
EndProc         MiniVDD_TurnVgaOff
;
;--------------------------------------------------------------------------
public  MiniVDD_TurnVgaOn
BeginProc       MiniVDD_TurnVgaOn, DOSVM

                push    eax
                push    esi

                ;We may not be mapped into the linear address space.
                ;If not, make it so.
                REG_RD32(NV_PBUS_PCI_NV_4)      ;get Bar 0 thru linear space
                cmp     eax,nvPhys              ;are we still here?
                jz      @f
                
                ;Go out to PCI space and reenable
                mov     edx,0cf8h
                mov     esi, offset callrm_struct
                mov     eax, dword ptr [esi].minivdd_RM_Parameter_Struct.callrm_irq_bus
                and     eax,0ffffff00h          ;clear lower byte (IRQ)
                or      eax,80000004h           ;enable config space mapping, reg 4 = command reg
                out     dx,eax
                mov     dx,0cfch
                in      eax,dx
                or      eax,3                   ;enable mem + I/O
                out     dx,eax
@@:                
                ;Enable NV PCI IO, bit0 = 1.
                REG_RD32(NV_PBUS_PCI_NV_1)
                or      al,01h
                REG_WR32(NV_PBUS_PCI_NV_1)

                ;Enable VGA through the VGA Enable IO register.
;This comment is here because the assembler was giving "line too long" error--comment fixes it. Very strange!
                REG_RD08(NV_PRMVIO_VSE2)
;
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
                mov     eax, NumUnits
                ret
EndProc         MiniVDD_GetNumUnits

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
;   findDevNode
;
;   DESCRIPTION:
;   Finds the array that matches the devNode.
;
;   ENTRY:
;       EBX = Devnode handle
;
;   EXIT:
;   Sets carry on error. On Success, sets ESI to point to the correct instance.
;
;   USES:
;       Can use all registers and flags
;
;==============================================================================
BeginProc findDevNode

    mov   esi, offset device1_instance
    .IF (dword ptr [esi].minivdd_instance_Struct.instance_devNode != ebx)
        mov   esi, offset device2_instance
        .IF (dword ptr [esi].minivdd_instance_Struct.instance_devNode != ebx)
            mov   esi, offset device3_instance
           .IF (dword ptr [esi].minivdd_instance_Struct.instance_devNode != ebx)
               mov   esi, offset device4_instance
              .IF (dword ptr [esi].minivdd_instance_Struct.instance_devNode != ebx)
                  mov   esi, offset device5_instance
                  .IF (dword ptr [esi].minivdd_instance_Struct.instance_devNode != ebx)
                      mov   esi, offset device6_instance
                      .IF (dword ptr [esi].minivdd_instance_Struct.instance_devNode != ebx)
                          mov   esi, offset device7_instance
                          .IF (dword ptr [esi].minivdd_instance_Struct.instance_devNode != ebx)
                              mov   esi, offset device8_instance
                              .IF (dword ptr [esi].minivdd_instance_Struct.instance_devNode != ebx)
                                  ; Did not find a match
                                  stc
                                  ret
                              .ENDIF
                          .ENDIF
                      .ENDIF
                  .ENDIF
              .ENDIF
           .ENDIF
        .ENDIF
    .ENDIF

    ; Found the devNode. ESI is setup to point to that instance
    clc
    ret
EndProc findDevNode

;==============================================================================
;   DoConfigRemove
;
;   DESCRIPTION:
;       Handles the CONFIG_REMOVE message. In reality it handles any message that
;   gives us a hint that the device is going to be removed. We never get a CONFIG_REMOVE!
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
BeginProc DoConfigRemove

    mov  devNode, ebx
    call findDevNode  ; ebx has the devnode.
    .IF (carry?)
       mov eax, CR_FAILURE
    .ELSE
       ; Found the device to be removed
       mov      eax, NVRM_API_EXIT          ; load up the function number
       mov      ebx, dword ptr [esi].minivdd_instance_Struct.instance_hDev
       call     CallRM                                      ; go to RM's API procedure

       ; Zero out the instance
       xor     ebx, ebx
       mov     dword ptr [esi].minivdd_instance_Struct.instance_devNode, ebx
       mov     dword ptr [esi].minivdd_instance_Struct.instance_nvAddr, ebx
       mov     dword ptr [esi].minivdd_instance_Struct.instance_nvPhys, ebx
       mov     dword ptr [esi].minivdd_instance_Struct.instance_fbPhys, ebx
       mov     dword ptr [esi].minivdd_instance_Struct.instance_fbLength, ebx
       mov     dword ptr [esi].minivdd_instance_Struct.instance_nvIRQ, ebx
       mov     dword ptr [esi].minivdd_instance_Struct.instance_hDev, ebx

       mov      eax, CR_SUCCESS

    .ENDIF


    ret
EndProc DoConfigRemove

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
        mov devNode, ebx

IF JIMK1
    mov     dwDisplayDevnodeHandle,ebx
ENDIF

;       call the Configuration Manager to get the device config.
        VxDcall _CONFIGMG_Get_Alloc_Log_Conf, <edi, ebx, CM_GET_ALLOC_LOG_CONF_ALLOC>
        test    eax, eax
        jz      @F
        Debug_Out "MiniVDD:  Unexpected config manager error #EAX"
        jmp     DoConfigStartDone

@@:

IF JIMK1
    ; Here we determine the local registry path
    push    edi

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
    mov     ebx,dwDisplayDevnodeHandle
    ; The local registry path is in szLocalDisplayPath
ENDIF

;       We got the device config; now parse the info and call the resource manager to map it.

        ;mov    esi, [dwRefData]        ; ESI -> adapter reference data

    ;
    ; NV3 should be given four or five memory windows
    ;
    ;   two for VGA (A0000 and B0000)
    ;   two for the PCI Bars (NV and FB)
    ;   one for AGP aperture/ROM (optional)
    ;
    ; We should find NV at index 2 and FB at index 3, but let's
    ; parse to make sure.
    ;
    ; Zero out the globals to start with.
    mov     nvPhys, 0
    mov     fbPhys, 0
    mov     fbLength, 0
    mov     nvIRQ, 0

    xor     ebx, ebx
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
    ; BUT -- we won't rely on this order.
        ; The config manager doesn't always return attrib = 5 (prefetchable) for fb, although this is set in PCI space,
        ; so we won't depend on it. Problem with win98 beta only? Maybe we can rely on this.
        ; We will test which window is which by reading some nv register in InitMapping (in the resource manager).
;    cmp     [edi.wMemAttrib][ebx*2], 0005h
    mov     eax, [edi.dMemBase][ebx*4]
;    je      @F
    mov     nvPhys, eax                ; nv has attrib of 1
    jmp     short start_2
;@@:
;    mov     fbPhys, eax                ; fb has attrib of 5 (PF)

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
    ; Found another 16/32 meg window.  Which one is it?
    ;
@@:
; Don't count on attrib = 5!
;    cmp     [edi.wMemAttrib][ebx*2], 0005h
    mov     eax, [edi.dMemBase][ebx*4]
;    je      @F
;       jne             @f                                                      ; yes, it must be fb
;    mov     nvPhys, eax                ; nv has attrib of 1
;    jmp     short start_3
    cmp     fbPhys, 0                   ; did we find fb already? (unlikely)
    je      @f
    mov     nvPhys, eax                 ; already have fb, must be nv
    jmp     start_3
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
    ; If none, go to PCI space and find it. We can do this in InitMapping, after nvAddr is mapped
    ;
;    mov     eax, CR_DEVICE_NOT_THERE    ; more than one irq?  ooops.
;       jmp             MiniVDD_PnP_Done
@@:
    xor     eax, eax
    mov     al, [edi.bIRQRegisters][0]
    mov     nvIRQ, eax

;       check that we got something (shouldn't ever be 0 unless PCI error)
    mov     eax, CR_DEVICE_NOT_THERE    ; assume failure
    cmp             nvPhys, 0
    jz              DoConfigStartDone
    cmp             fbPhys, 0
    jz              DoConfigStartDone
;   just getting the addresses is enough for now
;       cmp             _nvIRQ, 0
;       jz              DoConfigStartDone

;
;       Map the physical addresses to linear addresses and install interrupt
;       Addresses should be in correct order, but we'll check in InitMapping.
map_config:
    push    ebx
    push    ecx
    push    edx
    push    edi

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
    push    esi
    xor     esi, esi

    mov     esi, offset callrm_struct
    mov     dword ptr [esi].minivdd_RM_Parameter_Struct.callrm_device_handle, 2
    mov     dword ptr [esi].minivdd_RM_Parameter_Struct.callrm_modechange_callback, 0
    mov     dword ptr [esi].minivdd_RM_Parameter_Struct.callrm_irq_bus, edx
    mov     dword ptr [esi].minivdd_RM_Parameter_Struct.callrm_fbphys, ecx
    mov     dword ptr [esi].minivdd_RM_Parameter_Struct.callrm_nvphys, ebx
    lea     eax, szLocalDisplayPath
    mov     dword ptr [esi].minivdd_RM_Parameter_Struct.callrm_registry_path, eax
    mov     eax, NVRM_API_INIT                      ; load up the function number
    mov     ebx, DEFAULT_PRIMARY_HDEV
    call    CallRM                                  ; go to RM's API procedure
                                                    ; do mapping to logical addresses and installing interrupt
    pop     esi
    mov     nvAddr, ebx                 ; return with logical address to NV
    mov     hDev, ecx                   ; and a RM handle to the device.

    .IF     (eax == RM_OK)

        ; save off all these values for this devnode
        .if hDev == 1
          mov   esi, offset device1_instance
        .elseif hDev == 2
          mov   esi, offset device2_instance
        .elseif hDev == 3
          mov   esi, offset device3_instance
        .elseif hDev == 4
          mov   esi, offset device4_instance
        .elseif hDev == 5
          mov   esi, offset device5_instance
        .elseif hDev == 6
          mov   esi, offset device6_instance
        .elseif hDev == 7
          mov   esi, offset device7_instance
        .elseif hDev == 8
          mov   esi, offset device8_instance
        .else
          ; Too many devices !
          int 3h
        .endif

        mov     eax, devNode
        mov     dword ptr [esi].minivdd_instance_Struct.instance_devNode, eax

        mov     eax, nvAddr
        mov     dword ptr [esi].minivdd_instance_Struct.instance_nvAddr, eax
        mov     eax, nvPhys
        mov     dword ptr [esi].minivdd_instance_Struct.instance_nvPhys, eax
        mov     eax, fbPhys
        mov     dword ptr [esi].minivdd_instance_Struct.instance_fbPhys, eax
        mov     eax, fbLength
        mov     dword ptr [esi].minivdd_instance_Struct.instance_fbLength, eax
        mov     eax, nvIRQ
        mov     dword ptr [esi].minivdd_instance_Struct.instance_nvIRQ, eax
        mov     eax, hDev
        mov     dword ptr [esi].minivdd_instance_Struct.instance_hDev, eax

        mov     eax, CR_SUCCESS

    .ENDIF

    pop     edi
    pop             edx
    pop             ecx
    pop             ebx


;       end up here after success or fail
DoConfigStartDone:
    add     esp, SIZE Config_Buff_s                 ; adjust stack
    cmp     eax, CR_SUCCESS
    je      DoConfigStartExit

DoConfigStartError:
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

;       It seems CM_Register_Device_Driver causes a problem in the Win95 VDD. The VDD won't do the VESA DDC call (4F15).
;       Avoid doing it (CM_Register_Device_Driver) in Win95.

;   Well. It does not work on Win98 also! So Just do it the old way. The code to register for
;   config manager messages is left here commented out. Maybe someday Microsoft will fix this
;   and we can do it the documented way :-)

    VMMcall     Get_VMM_Version
    .IF (!(carry?) && (ax == 040ah))
       ; This is win98. Register to get config messages.
       VxDcall _CONFIGMG_Register_Device_Driver, <ebx, OFFSET32 MiniVDD_Config_Handler, 0, CM_REGISTER_DEVICE_DRIVER_DISABLEABLE OR CM_REGISTER_DEVICE_DRIVER_REMOVABLE>
           .ERRNZ       CR_SUCCESS
           test eax, eax
           jnz  SSND_Couldnt_Reg_Driver

       ; Register succeeded.
    .ENDIF

    ; Just do the init here for both win95 and win98.
    ; On win98, eventhough we get the CONFIG_START message, we get it multiple times
    ; for each devNode. To avoid hassles, just do the handling here and ignore the CONFIG_START
    ; messages.
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
       ; We should called twice with CONFIG_START. Once with a subfunction of CONFIG_START_FIRST_START
       ; and once with a subfunction of CONFIG_START_DYNAMIC_START.
       ; But both times we get CONFIG_START_FIRST_START. So the handling of this message is already
       ; done in pnp_new_devnode.
       mov eax, CR_SUCCESS

        .ELSEIF ([cfFuncName] == CONFIG_STOP)

       Debug_Out "MiniVDD:  PnP Stop"
       ; need to add code to handle this.
       mov eax, CR_SUCCESS

        .ELSEIF ([cfFuncName] == CONFIG_TEST)

           Debug_Out "MiniVDD:  PnP Test"
           mov  eax, CR_SUCCESS         ; Secondary device can be removed.

        .ELSEIF ([cfFuncName] == CONFIG_TEST_SUCCEEDED)

           Debug_Out "MiniVDD:  PnP Test succeeded"
       mov ebx, dnDevNode
       call DoConfigRemove

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

    .ELSEIF ([cfFuncName] == CONFIG_PRESHUTDOWN)
       Debug_Out "MiniVDD:  PnP PreShutdown"
       ; need to add code to handle this.
       mov eax, CR_SUCCESS

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

;               Nothing to do?
;               Tell the SysVM we did it successfully then
                clc
                ret
EndProc MiniVDD_Sys_VM_Terminate
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

MDDDExit:
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
EndProc MiniVDD_DisplayDriverDisabling
;

subttl          Register Display Driver Dependent Data
page +
public  MiniVDD_RegisterDisplayDriver
BeginProc MiniVDD_RegisterDisplayDriver, RARE
;
;Oft-times, the display driver must set flags whenever it is doing
;certain things to a hardware BLTer (such as "I'm in the middle of
;transferring a source bitmap").  This routine is called to allow
;this mini-VDD to get data (usually containing addresses inside the
;display driver's Data segment) directly from the display driver.
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
;NVIDIA: we use this function to pass the device handle to the resource manager. If running multiple devices, we need
;       to know which device (from the display driver's point of view), is associated with which address (the RM's
;       point of view). The miniVDD is the link between these entities.
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

MRDDExit:

;
        ; This is no longer used.
        ret                             ;
EndProc MiniVDD_RegisterDisplayDriver


;******************************************************************************
;       Any use for this function?
public  MiniVDD_GetTotalVRAMsize
BeginProc       MiniVDD_GetTotalVRAMsize, DOSVM
        push    eax
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
        pop     eax

                ret
EndProc         MiniVDD_GetTotalVRAMsize

;******************************************************************************
;
;   MiniVDD_GetAdpaterPowerState
;
;   DESCRIPTION:        ACPI power capabilities
;
;   ENTRY:  Device Node (stack)
;
;   EXIT:
;       EAX = Capabilities (bit 0-3 set if D0-D3 supported)
;
;==============================================================================
public  MiniVdd_GetAdapterPowerStateCaps
BeginProc MiniVdd_GetAdapterPowerStateCaps, RARE
;
;Entry:
;       DevNode.
;Exit:
;       The CAPS of adapter
        ArgVar  DeviceNode, DWORD               ;message number
        EnterProc
        cmp     OSVersion,VERS_MILLENNIUM       ;Win98 or Millennium?
        jb      OSVer_Win98
        mov     eax, (CM_POWERSTATE_D0 or CM_POWERSTATE_D3 or CM_POWERSTATE_D1 or CM_POWERSTATE_HIBERNATE)
        jmp     GAPSC_Done
OSVer_Win98:
        mov     eax, (CM_POWERSTATE_D0 or CM_POWERSTATE_D3 or CM_POWERSTATE_D1)
GAPSC_Done:
        LeaveProc
        Return
EndProc MiniVdd_GetAdapterPowerStateCaps

subttl          Set adapter Power State
page +

;******************************************************************************
;
;   MiniVDD_SetAdpaterPowerState
;
;   DESCRIPTION:        ACPI power state transition
;
;   ENTRY:  Device Node, Power State (stack)
;
;   EXIT:
;       EAX = CR_RESULT
;
;==============================================================================
public  MiniVdd_SetAdapterPowerState
BeginProc MiniVdd_SetAdapterPowerState, RARE
;Entry:
;       DevNode.
;       PowerState.
;Exit:
;       CR_DEFAULT  - OS should POST the device
;   CR_SUCCESS  - OS should do nothing
        ArgVar  DeviceNode, DWORD
        ArgVar  PowerState, DWORD
        ArgVar  DevInstance, DWORD

        EnterProc
        pushad
; Find the device instance and the current state
        mov     ebx,DeviceNode
        call    findDevNode
        jc      SAPS_Ret    ; find it?

        mov     ebx, [esi].minivdd_instance_Struct.instance_hDev
        mov     DevInstance,ebx
        mov     ebx, [esi].minivdd_instance_Struct.instance_ACPI_state
        mov     ebx,PowerState  ; requested state
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
        jnz     SAPS_D3

;   if we are going to state 3, we shut down.
;   if we are coming back to state 0, we must start up.
SAPS_D0:
        mov     ebx,PowerState
        cmp     [esi].minivdd_instance_Struct.instance_ACPI_state, ebx
        je      SAPS_Ret
        mov     [esi].minivdd_instance_Struct.instance_ACPI_state, ebx   ; save new state
;       call RM to start up (use API)
        mov     ebx,DevInstance     ; device instance
        mov     eax, NVRM_API_POWER_UP
        call    CallRM
        jmp     SAPS_Ret

SAPS_D3:
        mov     ebx,PowerState
        cmp     [esi].minivdd_instance_Struct.instance_ACPI_state, ebx
        je      SAPS_Ret
        mov     [esi].minivdd_instance_Struct.instance_ACPI_state, ebx   ; save new state
;       call RM to shut down (use API)
        mov     ebx,DevInstance     ; device instance
        mov     eax, NVRM_API_POWER_DOWN
        call    CallRM
        jmp     SAPS_Ret

SAPS_D2:
;   not supported
if 0
        mov     ebx,PowerState
        cmp     [esi].minivdd_instance_Struct.instance_ACPI_state, ebx
        je      SAPS_Ret
        mov     [esi].minivdd_instance_Struct.instance_ACPI_state, ebx   ; save new state
;       call RM to shut down (use API)
        mov     ebx,DevInstance     ; device instance
        mov     eax, NVRM_API_POWER_D2
        call    CallRM
endif
        jmp     SAPS_Ret

SAPS_D1:
        mov     ebx,PowerState
        cmp     [esi].minivdd_instance_Struct.instance_ACPI_state, ebx
        je      SAPS_Ret
        mov     [esi].minivdd_instance_Struct.instance_ACPI_state, ebx   ; save new state
;       call RM to shut down (use API)
        mov     ebx,DevInstance     ; device instance
        mov     eax, NVRM_API_POWER_D1
        call    CallRM

SAPS_Ret:
        popad
        mov     eax,CR_DEFAULT
        LeaveProc
        Return
EndProc MiniVdd_SetAdapterPowerState
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
        pop     ebp
        pop     eax                             ; res0
        pop     ebx
        pop     edx
        pop     ecx
        pop     eax
        ret
EndProc CALLRM

;
VxD_LOCKED_CODE_ENDS
;
;
end
