;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gninit.asm
;
; Purpose:      This file holds the DllEntryPoint code.
;
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include minivdd.inc
include gngdi.inc
include hwgdi.inc
include sysdata.inc
include gndata.inc
include gnlogdev.inc
include gnboard.inc
include gncrtc.inc
include escape.inc

OPENSEG  _DATA, USE16, DATA
szModeSetDll            DB      "nvmode.dll",0
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE
PUBLIC  CSlp16LogDevice
CSlp16LogDevice                 DD      0

PUBLIC  CSpGNLogicalDevice
CSpGNLogicalDevice              DD      0

PUBLIC  wNVTEXT32_Alias_In_NVTEXT32
wNVTEXT32_Alias_In_NVTEXT32     DW      0

PUBLIC  wNVTEXT16_Alias_In_NVTEXT32
wNVTEXT16_Alias_In_NVTEXT32     DW      0

PUBLIC  wFlatDataSel
wFlatDataSel                    DW      0
CLOSESEG _NVTEXT32


OPENSEG  _NVTEXT16, USE16, CODE
PUBLIC  CSlp16LogDevice_In_NVTEXT16
CSlp16LogDevice_In_NVTEXT16     DD      0

PUBLIC  CSpGNLogicalDevice_In_NVTEXT16
CSpGNLogicalDevice_In_NVTEXT16  DD      0

PUBLIC  wNVTEXT32_Alias_In_NVTEXT16
wNVTEXT32_Alias_In_NVTEXT16     DW      0

PUBLIC  wNVTEXT16_Alias_In_NVTEXT16
wNVTEXT16_Alias_In_NVTEXT16     DW      0

PUBLIC  wFlatDataSel_In_NVTEXT16
wFlatDataSel_In_NVTEXT16        DW      0

PUBLIC  ModesetTable
ModesetTable  LABEL   WORD
DW      OFFSET szModeSetDll, SEG szModeSetDll
hModesetDll                     DW              0
lpfnFindModeEntry                       MODULELINK      <0, 1>
lpfnFindVirtualModeEntry                MODULELINK      <0, 2>
lpfnGetModeList                         MODULELINK      <0, 3>
lpfnGetDesktopModeList                  MODULELINK      <0, 4>
lpfnSetVirtualDesktop                   MODULELINK      <0, 5>
lpfnGetVirtualDesktop                   MODULELINK      <0, 6>
lpfnSetDesktopState                     MODULELINK      <0, 7>
lpfnGetDesktopState                     MODULELINK      <0, 8>
lpfnGetBootDevice                       MODULELINK      <0, 9>
lpfnGetDisplayDevice                    MODULELINK      <0, 10>
lpfnSetDisplayDevice                    MODULELINK      <0, 11>
lpfnGetRefreshRateForDesktop            MODULELINK      <0, 12>
lpfnGetForcedDesktopMode                MODULELINK      <0, 13>
lpfnGetLargestEdidMode                  MODULELINK      <0, 14>
lpfnGetDeviceDesktopMode                MODULELINK      <0, 15>
lpfnSetDeviceDesktopMode                MODULELINK      <0, 16>
lpfnGetForcedDisplayMode                MODULELINK      <0, 17>
lpfnGetDeviceDisplayMode                MODULELINK      <0, 18>
lpfnSetDeviceDisplayMode                MODULELINK      <0, 19>
lpfnGetEdidRegistryKey                  MODULELINK      <0, 20>
lpfnGetLocalNvidiaDisplayRegValue       MODULELINK      <0, 21>
lpfnGetLocalNvidiaDisplayRegString      MODULELINK      <0, 22>
lpfnDeleteLocalRegEntry                 MODULELINK      <0, 23>
lpfnResetRefreshRates                   MODULELINK      <0, 24>
lpfnGetSafeTiming                       MODULELINK      <0, 25>
lpfnSetResolutionAndColorDepth          MODULELINK      <0, 26>
lpfnSetAutoPanMode                      MODULELINK      <0, 27>
lpfnGetAutoPanMode                      MODULELINK      <0, 28>
lpfnSetTimingOverride                   MODULELINK      <0, 29>
lpfnGetTimingOverride                   MODULELINK      <0, 30>
lpfnGetControlVisionFlag                MODULELINK      <0, 31>
lpfnGetBootDeviceSelection              MODULELINK      <0, 32>
lpfnIsDriverAttachedToDesktop           MODULELINK      <0, 33>
lpfnGetAllowAllModesFlag                MODULELINK      <0, 34>
lpfnGetAllowMightyModesFlag             MODULELINK      <0, 35>
lpfnGetCheckSXGAPanelFlag               MODULELINK      <0, 36>
lpfnGetClonePanningFlag                 MODULELINK      <0, 37>
lpfnSetClonePanningFlag                 MODULELINK      <0, 38>
lpfnDisplayDriverControl                MODULELINK      <0, 39>
lpfnFreeMasterLists                     MODULELINK      <0, 40>
lpfnGetCloneDesktopSelection            MODULELINK      <0, 41>
lpfnGetRestrictLargestMode              MODULELINK      <0, 42>
lpfnGet1400x1050Override                MODULELINK      <0, 43>
lpfnGetMaxDfpMode                       MODULELINK      <0, 44>
lpfnGetModesetStopImage                 MODULELINK      <0, 45>
                                        MODULELINK      <0, 0>

;==============================================================================
;
; Function:     DllEntryPoint
;
; Purpose:      The OS calls this function immediaetly after the DLL
;               loads. We don't do anything here.
;
; Arguments:    None
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC DllEntryPoint, PASCAL, NOFRAME, FAR16
OPENPROC
        mov     eax,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_DllEntryPoint
;
; Purpose:      This routine does all the global init for the driver.
;               It should be called by Enable the very first time the
;               driver loads.
;
; Arguments:
;               lpDstDevType            We can use this to get the
;                                       logical devic handle from the
;                                       minivdd
;               lpThisGdiInfo           GDIINFO ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_DllEntryPoint, PASCAL, FRAME, FAR16
PARMD   lpDstDevType
PARMD   lpThisGdiInfo
LOCALV  sDispInfo,DISPLAYINFO
LOCALD  dwDesktopState
LOCALD  pGNLogdevCXOffset
LOCALD  pGNBoardCXOffset
LOCALD  dwFirstBoardContext
OPENPROC
        PUSHR   ds,esi,edi

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Constants Creation
        push    lpDstDevType
        call    GN_CreateConstants
        or      eax,eax
        je      FailIt_00

IFDEF DEBUG
        ; Catch driver loading for the debugger
        mov     eax,1
        CALLFAR16IND lpfnDisplayDriverControl,es,bx
ENDIF

        call    HW_CreateConstants
        or      eax,eax
        je      FailIt_0

        ; This will keep throughout the routine
        mov     ds,cs:[wFlatDataSel_In_NVTEXT16]

        ; See if we already have an attachment to the board
        ; We need to get the board dev node handle. One way
        ; is through the GetDisplayInfo call.
        mov     ax,ss
        shl     eax,10H
        lea     ax,sDispInfo
        push    eax
        pushd   VDD_GET_DISPLAY_CONFIG
        call    GN_VddGetDisplayInfo

        ; See if a GNBOARDCX already exists for this board
        push    sDispInfo.diDevNodeHandle
        call    GN_FindBoardContext
        mov     dwFirstBoardContext,0
        or      eax,eax
        jne     DoneBoard

First:
        mov     dwFirstBoardContext,1

        ; Create the GNBOARDCX
        push    cs:[dwThisLogicalDeviceHandle_In_NVTEXT16]
        call    GN_CreateBoardContext
        or      eax,eax
        je      FailIt_1

DoneBoard:
        mov     pGNBoardCXOffset,eax

        ; Now create our logical device context
        push    pGNBoardCXOffset
        call    GN_CreateLogdevContext
        or      eax,eax
        je      FailIt_2
        mov     pGNLogdevCXOffset,eax

        ; We usually only need to attach children to their parents,
        ; but in this one special case we need to attach a parent
        ; to the child. Attach this logical device to the board.
        push    pGNBoardCXOffset
        push    pGNLogdevCXOffset
        call    GN_AttachLogicalDeviceToBoard
        or      eax,eax
        je      FailIt_3
        mov     esi,pGNLogdevCXOffset
        mov     ds:[esi].GNLOGDEVCX.dwLogicalDeviceInstanceOnThisBoard,ecx

        ; Se the GDIINFO ptr
        push    pGNLogdevCXOffset
        push    lpThisGdiInfo
        call    GN_SetGdiInfoPtr

        ; Save off the ptr
        mov     esi,pGNLogdevCXOffset
        mov     es,cs:[wNVTEXT16_Alias_In_NVTEXT16]
        mov     es:[CSpGNLogicalDevice_In_NVTEXT16],esi
        mov     es,cs:[wNVTEXT32_Alias_In_NVTEXT16]
        mov     es:[CSpGNLogicalDevice],esi

        ; The init can handle having more logical devices on the
        ; board than the board has CRTCs. This may seem weird,
        ; but abstractly it works. We limit it here as an external
        ; condition.
        mov     esi,cs:[CSpGNLogicalDevice_In_NVTEXT16]
        mov     eax,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        cmp     eax,ds:[esi].GNLOGDEVCX.dwLogicalDeviceInstanceOnThisBoard
        jbe     FailIt_4

        ; If the secondry driver is not enabled ("AttachedToDesktop"),
        ; then don't create any CRTCCXs since we will be unloaded immed.
        call    GN_VddIsDriverAttachedToDesktop
        or      ax,ax
        je      DoneClone

        ; Before we get any farther, figure out if we need to make
        ; modifications to the device connections. This is largely
        ; for mobile support.
        push    esi
        call    GN_ResolveDeviceConnectionsOnLoad

        ; Attach a CRTC to this logical device
        push    pGNLogdevCXOffset
        call    GN_LogdevAttachCrtc
        or      eax,eax
        je      FailIt_5

        ; Determine this driver's desktop state -- clone or standard.
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        mov     ax,ss
        shl     eax,10h
        lea     ax,dwDesktopState
        push    eax
        call    FFP16 PTR cs:[lpfnGetDesktopState]
        cmp     dwDesktopState,NVTWINVIEW_STATE_CLONE
        jne     DoneClone

        ; Attach a second CRTC to this logical device
        push    pGNLogdevCXOffset
        call    GN_LogdevAttachCrtc
        or      ax,ax
        jne     DoneClone

        ; Don't fail if there wasn't a device to use as a
        ; clone device. Just revert to standard mode.
        push    ds:[esi].GNLOGDEVCX.lpBoardRegistryData
        pushd   NVTWINVIEW_STATE_NORMAL
        CALLFAR16IND lpfnSetDesktopState,es,ax

DoneClone:
        ; Setup control offscreen allocations callbacks
        ; for the primary device
        cmp     dwThisLogicalDeviceHandle_In_NVTEXT16,1
        jne     Secondary
        call    GN_VddSetLockAndUnlockOffscreen
        call    GN_VddSetPowerManagementCallbacks

Secondary:
        mov     eax,1
        jmp     DoneIt

FailIt_6:
FailIt_5:
FailIt_4:
        push    pGNBoardCXOffset
        push    pGNLogdevCXOffset
        call    GN_DetachLogicalDeviceFromBoard

FailIt_3:
        push    pGNLogdevCXOffset
        call    GN_DestroyLogdevContext

FailIt_2:
        cmp     dwFirstBoardContext,0
        je      FailIt_1
        push    pGNBoardCXOffset
        call    GN_DestroyBoardContext

FailIt_1:
        push    dwThisLogicalDeviceHandle_In_NVTEXT16
        call    HW_DestroyConstants

FailIt_0:
        call    GN_DestroyConstants
        sub     eax,eax

FailIt_00:
DoneIt:
        POPR    ds,esi,edi
CLOSEPROC



;==============================================================================
;
; Function:     GENERIC_WEP
;
; Purpose:      This function is the WEP entry point.
;
; Arguments:    None
;
; Returns:      None
;
; Preserve      ds,esi,edi
;==============================================================================
PUBLIC  WEP
WEP     PROC    FAR16
        ; Windows requires that WEP be exported by name. I have the
        ; real routine GENERIC_WEP equated to the same address.
        .errnz  $ - OFFSET GENERIC_WEP
WEP     ENDP

DECPROC GENERIC_WEP, PASCAL, FRAME, FAR16
LOCALD  dwCRTCCount
LOCALD  dwLogdevAttachedToBoard
LOCALD  pGNBoardContext
LOCALD  pGNBoardCXOffset
LOCALD  dwDriverEnabled
OPENPROC
        PUSHR   ds,esi,edi

IFDEF DEBUG
        ; Catch driver unloading for the debugger
        mov     eax,0
        CALLFAR16IND lpfnDisplayDriverControl,es,bx
ENDIF

        ; Check to see how far in the creation process we got.
        ; We are guaranteed from the way GN_DllEntryPoint is
        ; written that if wNVTEXT16_Alias_In_NVTEXT16 is non-0,
        ; then we have a GNLOGDEVCX created.
        cmp     cs:[wNVTEXT16_Alias_In_NVTEXT16],0
        je      DoneIt

        mov     ds,cs:[wFlatDataSel_In_NVTEXT16]
        mov     esi,CSpGNLogicalDevice_In_NVTEXT16

        ; Sometimes disable doesn't get called, but WEP does
        ; and we see a flash of garbage when shutting down.
        ; This may prevent that. Windows clears the AttachToDesktop
        ; flag before calling WEP!! So, check to see if we
        ; created a primary CRTCCX to know if this driver was
        ; really enabled or not -- unh!
        mov     eax,dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX
        mov     dwDriverEnabled,eax

        cmp     dwDriverEnabled,0
        je      @F
        push    esi
        call    GN_BlankScreen
@@:
        ; Tell everyone that we are unloading.
        sub     eax,eax
        push    esi
        pushd   MSG_DISPLAY_DRIVER_UNLOADING
        push    eax
        push    eax
        call    GN_LogdevMessage

        mov     eax,ds:[esi].GNLOGDEVCX.pGNBoardCX
        mov     pGNBoardCXOffset,eax

        ; One less logical device on this board
        push    pGNBoardCXOffset
        push    esi
        call    GN_DetachLogicalDeviceFromBoard
        mov     dwLogdevAttachedToBoard,eax

        cmp     dwDriverEnabled,0
        je      @F
        cmp     dwLogdevAttachedToBoard,0
        je      @F
        ; The secondary driver is being unloaded. Tell ResMan we are
        ; leaving spanning mode.
        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        pushd   0
        call    HW_TellResManSpanningState
@@:
        ; Destroy the logical device context
        mov     eax,ds:[esi].GNLOGDEVCX.pGNBoardCX
        mov     pGNBoardContext,eax
        push    esi
        call    GN_DestroyLogdevContext

        cmp     dwDriverEnabled,0
        je      @F
        cmp     dwLogdevAttachedToBoard,0
        je      @F
        ; We might want to change display device configurations now
        mov     eax,pGNBoardCXOffset
        push    dword ptr ds:[eax].GNBOARDCX.pGNLogdevCX
        call    GN_ResolveDeviceConnectionsOnUnload
@@:
        cmp     dwLogdevAttachedToBoard,0
        jne     @F
        push    pGNBoardContext
        call    GN_DestroyBoardContext
@@:
        push    dwThisLogicalDeviceHandle_In_NVTEXT16
        call    HW_DestroyConstants

        call    GN_DestroyConstants

DoneIt:
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_CreateConstants
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
DECPROC GN_CreateConstants, PASCAL, FRAME, FAR16
PARMD   lpDstDevType
OPENPROC
        PUSHR   ds

        ; Allocate a writable alias for the 16bit code segment.
        mov     ax,SEG wNVTEXT16_Alias_In_NVTEXT16
        push    ax
        call    AllocCSToDSAlias
        or      ax,ax
        je      Done
        mov     ds,ax
        mov     ds:[wNVTEXT16_Alias_In_NVTEXT16],ax

        ; Allocate a writable alias for the 32bit code segment
        mov     ax,SEG wNVTEXT16_Alias_In_NVTEXT32
        push    ax
        call    AllocCSToDSAlias
        or      ax,ax
        je      Done
        mov     es,ax
        mov     es:[wNVTEXT32_Alias_In_NVTEXT32],ax

        ; Now put each in the other
        mov     ax,es:[wNVTEXT32_Alias_In_NVTEXT32]
        mov     ds:[wNVTEXT32_Alias_In_NVTEXT16],ax
        mov     ax,ds:[wNVTEXT16_Alias_In_NVTEXT16]
        mov     es:[wNVTEXT16_Alias_In_NVTEXT32],ax

        ; The VDDEntryPoint is just an address that is constant
        call    GN_VddGetEntryPoint

        ; Let's get the device handle so we can get a unique
        ; board identifier.
        push    lpDstDevType
        call    GN_VddGetDeviceHandle
        or      eax,eax
        je      Done

        lea     esi,ModesetTable
        call    GN_LoadDllAndGetExports
Done:
        POPR    ds
CLOSEPROC


;==============================================================================
;
; Function:     GN_DestroyConstants
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
DECPROC GN_DestroyConstants, PASCAL, NOFRAME, FAR16
OPENPROC

        ; Make sure that none of the selectors
        ; has any selectors we are about to free in them.
        sub     ax,ax
        mov     ds,ax
        mov     es,ax
        mov     fs,ax
        mov     gs,ax

        ; Did we even create an NVTEXT32 Code Alias?
        mov     ax,cs:[wNVTEXT32_Alias_In_NVTEXT16]
        or      ax,ax
        je      Done32

        ; Zero out the variables in this code segment.
        mov     ds,ax
        mov     ds:[CSpGNLogicalDevice],0
        mov     ds:[wNVTEXT16_Alias_In_NVTEXT32],0

        sub     eax,eax
        mov     ds:[wNVTEXT32_Alias_In_NVTEXT32],ax
        mov     cx,ds
        mov     ds,ax

        ; Free our writable NVTEXT32 code segment alias
        push    cx
        call    FreeSelector
Done32:

        ; Did we even create a NVTEXT16 Code Alias?
        mov     ax,cs:[wNVTEXT16_Alias_In_NVTEXT16]
        or      ax,ax
        je      Done16
        mov     ds,ax

        ; Zero out the variables in this code segment.
        mov     ds:[CSpGNLogicalDevice_In_NVTEXT16],0
        mov     ds:[wNVTEXT32_Alias_In_NVTEXT16],0

        sub     eax,eax
        mov     ds:[wNVTEXT16_Alias_In_NVTEXT16],ax
        mov     cx,ds
        mov     ds,ax

        ; Free our writable NVTEXT16 code segment alias
        push    cx
        call    FreeSelector
Done16:
CLOSEPROC


;==============================================================================
;
; Function:     GN_LoadDllAndGetExports
;
; Purpose:      This function loads the NvSys 16-bit RM interface dll
;               and gets addresses to the exports.
;
; Arguments:
;               ds:esi  offset of a MODULELINK array
;
; Returns:      eax     0       then load or export grab failed
;                       non-0   then everything was a success
;
; Preserve:     ds,esi
;==============================================================================
DECPROC GN_LoadDllAndGetExports, PASCAL, FRAME, FAR16
LOCALW  hDll
OPENPROC
        PUSHR   ds,esi,edi      ; LoadLibrary trashes DI and SI

        ; Load the library.  If it fails to load, we are totally hosed
        ; and there is no way to recover so blow off error code check.
        PUSHR   esi
        push    dword ptr ds:[esi]
        call    LoadLibrary
        POPR    esi
        mov     hDll,ax
        mov     ds:[esi + 4],ax
        add     esi,6

        ; Get some proc address
@@:     push    hDll
        pushw   0
        push    ds:[esi].MODULELINK.wOrdinal
        call    GetProcAddress
        shl     edx,10H
        mov     dx,ax
        mov     ds:[esi].MODULELINK.lpfnFunc,edx
        add     esi,SIZE MODULELINK
        add     edi,2
        cmp     word ptr ds:[esi].MODULELINK.wOrdinal,0
        jne     @B

        mov     eax,1
        POPR    ds,esi,edi
CLOSEPROC

CLOSESEG _NVTEXT16

END     DllEntryPoint

