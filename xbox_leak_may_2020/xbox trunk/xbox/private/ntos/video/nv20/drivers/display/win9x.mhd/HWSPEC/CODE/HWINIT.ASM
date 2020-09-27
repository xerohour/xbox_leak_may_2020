;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         hwinit.asm
;
; Purpose:      This file does hardware specific initialization that
;               needs to be done before anything else in the driver
;               executes. Also, it holds the routine that gets called
;               just before the driver DLL unloads (the WEP) routine.
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include gngdi.inc
include hwgdi.inc
include files.inc
include hwmacs.inc
include nvcm.inc
include nvreg.inc
include sysdata.inc
include nv32.inc
include hwlogdev.inc
include hwboard.inc
include hwcrtc.inc
include ..\..\..\..\..\sdk\nvidia\inc\nvrmarch.inc
include ..\..\..\..\..\sdk\nvidia\inc\rddapi.inc

OPENSEG  _DATA, USE16, DATA
szNvSysDll              DB      NV4_RM_16_BINARY,0
CLOSESEG _DATA


OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE
PUBLIC  NvSysTable
NvSysTable      LABEL   WORD
DW      OFFSET szNvSysDll, SEG szNvSysDll
hNvSysDll                       DW      0
lpfnNvSysMoveCursor             MODULELINK      <0, ORD_NvSysMoveCursorMulti>
lpfnNvSysUpdateImageOffsetFormat MODULELINK     <0, ORD_NvSysUpdateImageOffsetFormat>
lpfnNvSysGetRamInfo             MODULELINK      <0, ORD_NvSysGetRamInfo>
lpfnNvSysAllocPages             MODULELINK      <0, ORD_NvSysAllocPages>
lpfnNvSysFreePages              MODULELINK      <0, ORD_NvSysFreePages>
lpfnNvConfigGet                 MODULELINK      <0, ORD_NvConfigGet>
lpfnNvConfigSet                 MODULELINK      <0, ORD_NvConfigSet>
lpfnNvConfigUpdate              MODULELINK      <0, ORD_NvConfigUpdate>
lpfnNvConfigVga                 MODULELINK      <0, ORD_NvConfigVga>
lpfnNvSysSetCursorImage         MODULELINK      <0, ORD_NvSysSetCursorImageMulti>
lpfnNvSysGetHardwarePointers    MODULELINK      <0, ORD_NvSysGetHardwarePointers>
lpfnNvDeviceBaseGet             MODULELINK      <0, ORD_NvDeviceBaseGet>
lpfnNvDeviceLimitGet            MODULELINK      <0, ORD_NvDeviceLimitGet>
lpfnNvSysDriverConnect          MODULELINK      <0, ORD_NvSysDriverConnect>
lpfnNvSysDriverDisConnect       MODULELINK      <0, ORD_NvSysDriverDisConnect>
lpfnNvSysRegisterDisplayDriver  MODULELINK      <0, ORD_NvSysRegisterDisplayDriver>
lpfnNvIoControl                 MODULELINK      <0, ORD_NvIoControl>
lpfnArch_Nv01Free               MODULELINK      <0, ORD_Nv01Free>
lpfnArch_Nv01AllocRoot          MODULELINK      <0, ORD_Nv01AllocRoot>
lpfnArch_Nv01AllocMemory        MODULELINK      <0, ORD_Nv01AllocMemory>
lpfnArch_Nv01AllocContextDma    MODULELINK      <0, ORD_Nv01AllocContextDma>
lpfnArch_Nv01AllocChannelPio    MODULELINK      <0, ORD_Nv01AllocChannelPio>
lpfnArch_Nv01AllocObject        MODULELINK      <0, ORD_Nv01AllocObject>
lpfnArch_Nv01AllocDevice        MODULELINK      <0, ORD_Nv01AllocDevice>
lpfnArch_Nv03AllocChannelDma    MODULELINK      <0, ORD_Nv03AllocChannelDma>
lpfnArch_Nv03DmaFlowControl     MODULELINK      <0, ORD_Nv03DmaFlowControl>
lpfnArch_Nv01Interrupt          MODULELINK      <0, ORD_Nv01Interrupt>
lpfnArch_Nv01AllocEvent         MODULELINK      <0, ORD_Nv01AllocEvent>
lpfnArch_NvI2CAccess            MODULELINK      <0, ORD_NvI2CAccess>
lpfnArch_Nv04ConfigGetEx        MODULELINK      <0, ORD_Nv04ConfigGetEx>
lpfnArch_Nv04ConfigSetEx        MODULELINK      <0, ORD_Nv04ConfigSetEx>
lpfnArch_Nv04Alloc              MODULELINK      <0, ORD_Nv04Alloc>
                                MODULELINK      <0, 0>
CLOSESEG _NVTEXT32

OPENSEG  _NVTEXT16, USE16, CODE
;==============================================================================
;
; Function:     HW_CreateConstants
;
; Purpose:      This functions should init any things that are
;               effectively constants in the driver. Usually these
;               are items that go in the code segment. This routine
;               is called only once at boot and no context has yet
;               been created.
;
; Arguments:    None
;
; Returns:      eax     0       Failure
;                       non-0   Success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_CreateConstants, PASCAL, FRAME, FAR16
OPENPROC
        PUSHR   ds,esi
        mov     ds,wNVTEXT32_Alias_In_NVTEXT16
        lea     esi,NvSysTable
        call    GN_LoadDllAndGetExports
        POPR    ds,esi
CLOSEPROC


;==============================================================================
;
; Function:     HW_DestroyConstants
;
; Purpose:      This functions should destroy any "constants"
;               created by the CreateConstants routine. This
;               routine is called once when the driver is about
;               to be unloaded.
;
; Arguments:    None
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_DestroyConstants, PASCAL, FRAME, FAR16
PARMD   dwThisLogDeviceHandle
OPENPROC
        ; In multi-monitor applications, the secondary device must not
        ; attempt to unload the Dlls, otherwise the system will gpf on
        ; a windows 'quick-boot' (shift-enter on restart). We are
        ; assuming that the primary device has a device handle of 1.
        cmp     dwThisLogDeviceHandle,1
        jne     SkipDllUnload

        ; Unload the RM 16bit dll.
        mov     es,wNVTEXT32_Alias_In_NVTEXT16
        push    es:[hNvSysDll]
        mov     es:[hNvSysDll],0
        call    FreeLibrary
SkipDllUnload:
CLOSEPROC


;==============================================================================
;
; Function:     HW_AllocFlatMemory
;
; Purpose:      This routine buys memory in VDD land (3Gig-4Gig),
;               zeros it out and returns a flat ptr to it.
;
; Arguments:
;               dwAmount                How much to buy
;
; Returns:
;               eax     Flat ptr to memory block
;                       0 if failure
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_AllocFlatMemory, PASCAL, FRAME, FAR16
PARMD   dwAmount
LOCALV  rmRegs,RMPARAMS
OPENPROC
        mov     rmRegs.RegECX,MVDD_ALLOC_MEMORY_BLOCK
        mov     eax,dwAmount
        mov     rmRegs.RegEDX,eax
        mov     ax,ss
        lea     ebx,rmRegs
        push    ax
        push    bx
        CALLFAR16IND lpfnNvSysRegisterDisplayDriver,es,ax
        mov     eax,rmRegs.RegEAX
CLOSEPROC


;==============================================================================
;
; Function:     HW_FreeFlatMemory
;
; Purpose:      This routine frees a memory block allocated
;               via HW_AllocFlatMemory
;
; Arguments:
;               pFlatMem                ptr to memory to free
;
; Returns:
;               eax     Flat ptr to memory block
;                       0 if failure
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_FreeFlatMemory, PASCAL, FRAME, FAR16
PARMD   pFlatMem
LOCALV  rmRegs,RMPARAMS
OPENPROC
        mov     rmRegs.RegECX,MVDD_FREE_MEMORY_BLOCK
        mov     eax,pFlatMem
        mov     rmRegs.RegEDX,eax
        mov     ax,ss
        lea     ebx,rmRegs
        push    ax
        push    bx
        CALLFAR16IND lpfnNvSysRegisterDisplayDriver,es,ax
CLOSEPROC


;==============================================================================
;
; Function:     HW_GetLogdevToBoardPtr
;
; Purpose:      This function gets the shared block of memory used
;               for managing logical device to board instance stuff.
;               The memory must be shared across all driver contexts.
;
; Arguments:	None
;
; Returns:      eax                     context ptr to shared memory
;               THIS ROUTINE CANNOT FAIL.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_GetLogdevToBoardPtr, PASCAL, FRAME, FAR16
LOCALV  rmRegs,RMPARAMS
OPENPROC
        mov     rmRegs.RegECX,MVDD_GET_LOGDEV_SHARED_MEMORY
        mov     ax,ss
        lea     ebx,rmRegs
        push    ax
        push    bx
        CALLFAR16IND lpfnNvSysRegisterDisplayDriver,es,ax
        mov     eax,rmRegs.RegEAX
CLOSEPROC

CLOSESEG _NVTEXT16

END

