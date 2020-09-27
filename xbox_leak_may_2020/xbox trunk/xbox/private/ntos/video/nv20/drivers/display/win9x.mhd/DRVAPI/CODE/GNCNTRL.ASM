;==============================================================================
;
; Copyright (C) 1998, Nvidia Coporation
;
; File:         gncntrl.asm
;
; Purpose:      This file holds the Control display driver entry point.
;
;==============================================================================
.586
incDrawMode = 1
incDevice   = 1
incLogical  = 1
include macros.dat
include gdidefs.inc
include dibeng.inc
include sysdata.inc
include gndata.inc
include gnlogdev.inc
include gnboard.inc
include gncrtc.inc
include gngdi.inc
include hwgdi.inc
include valmode.inc
include escape.inc
include tvout.inc
include modeext.inc

CONTROL_FRAME_AND_LOCALS        MACRO
PARMD   lpDstDev
PARMW   wFunction
PARMD   lpIn
PARMD   lpOut
LOCALD  dwCRTCCount
LOCALD  dwCopyStandard
LOCALD  pGNCrtcCXOffset
LOCALD  pHWCrtcCXOffset
LOCALD  pGNLogdevCXUsingTheHead
LOCALD  pGNCrtcCXUsingTheHead
LOCALD  pHWCrtcCXUsingTheHead
LOCALD  dwLogCrtcIndex
ENDM

QUERYESCSUPPORT         TEXTEQU <8>
MOUSETRAILS             TEXTEQU <39>
DCICOMMAND              TEXTEQU <3075>
EDDESCAPE                               TEXTEQU <0EEEE0000H>
EDDESC_SCROLL                   TEXTEQU <0EDD00012H>

EDDESC_SCROLL_DATA      struc
dwSize          dd      ?               ;// = sizeof(struct tagSCROLL_DATA)
dwCommand       dd      ?               ;// = EDDESC_SCROLL
cyLines         dd      ?               ;// # of lines vertical panning
EDDESC_SCROLL_DATA      ends

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE

;==============================================================================
;
; Function:     GENERIC_Control
;
; Purpose:      This function is the Control display driver entry point.
;               It will handle all the Windows defined escapes. Any non-
;               Windows escapes will be passed off to the hardware
;               specific routine.
;
; Arguments:    Control Display Driver Stack Frame
;
; Returns:      The return value depends upon the specific control call.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_Control, PASCAL, FRAME, FAR16
CONTROL_FRAME_AND_LOCALS
OPENPROC
        PUSHR   ds,esi,edi
        pushf
        cld

        ; We want a ptr to the GNLOGDEVCX passed in (which is
        ; not necessarily the one corresponding to this driver!
        mov     ds,cs:[wFlatDataSel_In_NVTEXT16]
        mov     eax,lpDstDev
        shr     eax,10H
        push    eax
        call    GN_GetSelectorBase
        mov     esi,eax

        movzx   eax,wFunction
        push    OFFSET RetAddr
        cmp     eax,QUERYESCSUPPORT
        je      GN_ControlQuery
        cmp     eax,DCICOMMAND
        je      GN_ControlDDraw
        cmp     eax,MOUSETRAILS
        je      GN_MouseTrails
        add     sp,@WordSize

        push    OFFSET Done
        cmp     eax,GN_ESC_GET_PDEVICE
        je      GN_ControlReturnPDevice
        cmp     eax,GN_ESC_ALLOC_SELECTOR_AND_MAP
        je      GN_ControlAllocSelectorAndMap
        cmp     eax,GN_ESC_UNMAP_AND_FREE_SELECTOR
        je      GN_ControlUnmapAndFreeSelector
        cmp     eax,NV_ESC_ALLOC_SHARED_MEMORY
        je      GN_ControlAllocSharedMemory
        cmp     eax,NV_ESC_FREE_SHARED_MEMORY
        je      GN_ControlFreeSharedMemory
        cmp     eax,NV_ESC_GET_SHARED_MEMORY
        je      GN_ControlGetSharedMemory
        cmp     eax,GN_ESC_SET_CURSOR
        je      GN_ControlSetCursor
        cmp     eax,NV_ESC_DVD_MACROVISION_CMD
        je      GN_DvdMacrovisionCommand
        cmp     eax,OPENGL_GETINFO
        je      GN_OpenGLGetInfo
        cmp     eax,VIDEO_PARAMETERS
        je      GN_VideoParameters
        cmp     eax,NV_ESC_GET_LOCAL_REGISTRY_PATH
        je      GN_GetLocalRegistryPath
        cmp     eax,NV_ESC_GET_REGISTRY_MODE_LIST
        je      GN_GetRegistryModeList
        cmp     eax,NV_ESC_PRIMARY_INFO
        je      GN_GetPrimaryInfo
        cmp     eax,NV_ESC_GET_REGISTRY_DESKTOP_MODE_LIST
        je      GN_GetRegistryDesktopModeList
        cmp     eax,ESC_NV_QUERY_HEAD_REFRESH_RATE
        je      GN_QueryHeadRefreshRate
        cmp     eax,NV_ESC_ENABLE_DISABLE_SOFTWARE_CURSOR
        je      GN_SoftwareCursor
        cmp	eax,NV_ESC_SET_CPL_DWORD
        je      GN_SetCplDword
        cmp	eax,NV_ESC_GET_CPL_DWORD
        je      GN_GetCplDword
        add     sp,@WordSize

        ; See if it is a dualhead specific escape
        push    esi                     ; GNLOGDEVCX
        push    lpDstDev
        push    wFunction
        push    lpIn
        push    lpOut
        call    ControlPanelInterface
        or      ecx,ecx
        jne     Done

        ; See if it is a hardware specific escape
        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        push    lpDstDev
        push    wFunction
        push    lpIn
        push    lpOut
        call    HW_ControlAction
        or      ecx,ecx
        jne     Done

BackPunt:
        ; Punt it to DIBENG
        push    lpDstDev
        push    wFunction
        push    lpIn
        push    lpOut
        call    DIB_Control
        jmp     Done

RetAddr:
        or      ecx,ecx
        je      BackPunt
Done:
        popf
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     GN_ControlQuery
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is QUERYESCSUPPORT. This routine should
;               see first if it is a generic escape it knows about and,
;               if so, it should return the appropriate return code. If
;               it is not a generic escape, this routine will call the
;               hardware specific query routine to see if it is a HW
;               specific escape.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      ecx     0   escape was not recognized
;                       1   escape was recognized
;                           in this case, ax has the value to return to GDI
;
; Preserve:     ds:esi
;==============================================================================
DECPROC GN_ControlQuery, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        sub     ebx,ebx
        les     bx,lpIn
        mov     ecx,1
        movzx   edx,word ptr es:[ebx]

        cmp     edx,DCICOMMAND
        jne     @F
        mov     eax,(DD_HAL_VERSION - 1)        ; WHQL certification "bit"
        jmp     Done
@@:
        cmp     edx,OPENGL_GETINFO
        jne     @F
        mov     eax,1
        jmp     Done
@@:
        cmp     edx,OPENGL_CMD
        jne     @F
        sub     eax,eax
        jmp     Done
@@:
        cmp     edx,VIDEO_PARAMETERS
        jne     @F
        mov     eax,1
        jmp     Done
@@:
        cmp     edx,NV_ESC_DVD_MACROVISION_CMD
        jne     @F
        mov     eax,1
        jmp     Done
@@:
        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        push    lpDstDev
        push    wFunction
        push    lpIn
        push    lpOut
        call    HW_ControlQuery
        or      ecx,ecx
        jne     Done

        ; Punt it to DIBENG
        push    lpDstDev
        push    wFunction
        push    lpIn
        push    lpOut
        call    DIB_Control
Done:
CLOSEPROC


;==============================================================================
;
; Function:     GN_ControlDDraw
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is DCICOMMAND. This routine will
;               handle the flavors of DCICOMMAND escapes it knows about
;               and punt the rest to the DIBENG.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      ecx     0   DCICOMMAND sub-function was not recognized
;                       1   DCICOMMAND sub-function was recognized
;                           in this case, ax has the value to return to GDI
;
; Preserve:
;==============================================================================
DECPROC GN_ControlDDraw, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        sub     ebx,ebx

        ; Punt if lpIn is NULL or if the version is wrong
        cmp     ebx,lpIn
        je      Punt
        les     bx,lpIn
        cmp     es:[ebx].MYDCICMD.dwVersion,DD_VERSION
        jne     Punt

        push    OFFSET Handled
        mov     eax,es:[ebx].MYDCICMD.dwCommand
        cmp     eax,DDNEWCALLBACKFNS
        je      GN_ControlDDrawNewCallbackFns
        cmp     eax,DDGET32BITDRIVERNAME
        je      GN_ControlDDrawGet32BitDriverName
        cmp     eax,DDCREATEDRIVEROBJECT
        je      GN_ControlDDrawCreateDriverObject
        cmp     eax,DDVERSIONINFO
        je      GN_ControlDDrawDDVersionInfo

        ; These are not really standard DCI escapes. The
        ; DDNVCREATEDRIVEROBJECT escape tells us that the DirectX
        ; driver just went from having no apps attached to having one
        ; app attached. The DDNVDESTROYDRIVEROBJECT tells us that
        ; Direct X just went from having 1 app attached to having no
        ; apps attached.
        cmp     eax,DDNVCREATEDRIVEROBJECT
        je      GN_ControlDDrawNVCreateDriverObject
        cmp     eax,DDNVDESTROYDRIVEROBJECT
        je      GN_ControlDDrawNVDestroyDriverObject

        ; For a little applet someone wrote.
        cmp     eax,EDDESCAPE
        je      GN_RollerToolEscape
        add     sp,@WordSize

Punt:
        sub     ecx,ecx
        jmp     Done

Handled:
        mov     ecx,1
Done:
CLOSEPROC


;==============================================================================
;
; Function:     GN_ControlDDrawNewCallbackFns
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is DCICOMMAND and the subfunction
;               is DDNEWCALLBACKFNS.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      TRUE
;
; Preserve:
;==============================================================================
DECPROC GN_ControlDDrawNewCallbackFns, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; Save the callbacks to internal direct functions
        sub     ebx,ebx
        les     bx,lpIn
        mov     eax,es:[ebx].MYDCICMD.dwParam1

        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        push    eax
        call    HW_SetNewDirxCallbacks

        mov     eax,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_ControlDDrawGet32BitDriverName
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is DCICOMMAND and the subfunction
;               is DDGET32BITDRIVERNAME.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      TRUE
;
; Preserve:
;==============================================================================
DECPROC GN_ControlDDrawGet32BitDriverName, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; Copy the Direct Draw Dll Name into the lpOut structure
        ; so the system knows which Dll to load.
        movzx   ebx,word ptr lpOut+0
        movzx   eax,word ptr lpOut+2
        lea     edx,[ebx].MYDD32BITDRIVERDATA.szName
        lea     ecx,[ebx].MYDD32BITDRIVERDATA.szEntryPoint

        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        push    eax
        push    edx
        push    eax
        push    ecx
        call    HW_GetDirectXDllNameAndEntryPoint

        sub     ebx,ebx
        les     bx,lpOut
        mov     es:[ebx].MYDD32BITDRIVERDATA.dwContext,eax
        mov     eax,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_ControlDDrawCreateDriverObject
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is DCICOMMAND and the subfunction
;               is DDCREATEDRIVEROBJECT.
;
;               NOTE: This function may get called in a DISABLED
;               secondary display driver in which case we will
;               never actually receive a DD
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      TRUE
;
; Preserve:
;==============================================================================
DECPROC GN_ControlDDrawCreateDriverObject, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; Let everyone know that DirectX is at least enumerating.
        push    esi
        pushd   MSG_DIRECTX_ENUM
        push    esi
        push    eax                             ; ignored
        call    GN_LogdevMessage

        ; We are supposed to return the hInstance of our driver for this
        ; sub-escape. If we don't know it, then return 0.
        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        call    HW_GetDirectXInstance
        sub     ebx,ebx
        les     bx,lpOut
        mov     es:[ebx],eax

        ; Success code
        mov     eax,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_ControlDDrawNVCreateDriverObject
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is DCICOMMAND and the subfunction
;               is DDNVCREATEDRIVEROBJECT.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      TRUE
;
; Preserve:
;==============================================================================
DECPROC GN_ControlDDrawNVCreateDriverObject, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; Let everyone know that DirectX is starting up.
        push    esi
        pushd   MSG_DIRECTXON
        push    esi
        push    eax                             ; ignored
        call    GN_LogdevMessage

        ; Need to remember that directX is active
        mov     ds:[esi].GNLOGDEVCX.bDirectXActive,1

Done:
        ; Return success and handled
        mov     eax,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_ControlDDrawNVDestroyDriverObject
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is DCICOMMAND and the subfunction
;               is DDNVDESTROYDRIVEROBJECT.
;
;		NOTE: Due to a messed up DirectX run-time, we
;		can get multiple creates and only one destroy.
;		When we get the destroy we need to shut down
;		DirectX on ALL logical devices, not just the
;		one corresponding to the driver we are called in.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      TRUE
;
; Preserve:
;==============================================================================
DECPROC GN_ControlDDrawNVDestroyDriverObject, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
	; Inform all GNLOGDEVCXs on this board thwt DirectX is going away
	mov	dwCRTCCount,0
LoopT:
	mov	ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
	mov	eax,dwCRTCCount
	mov	edi,ds:[ebx].GNBOARDCX.pGNLogdevCX[eax * 4]
	or	edi,edi
	je	@F

        push    edi
        pushd   MSG_DIRECTXOFF
        push    edi
        push    eax                             ; ignored
        call    GN_LogdevMessage

        ; Need to remember that directX is not active
        mov     ds:[edi].GNLOGDEVCX.bDirectXActive,0

@@:
	inc	dwCRTCCount
	mov	eax,dwCRTCCount
	mov	ebx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        cmp     eax,ds:[ebx].GNBOARDCX.dwNumCrtcsOnBoard
        jc      LoopT

Done:
        ; Return success and handled
        mov     eax,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_ControlDDrawDDVersionInfo
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is DCICOMMAND and the subfunction
;               is DDVERSIONINFO.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      eax     0       failure
;                       non-0   Success
;
; Preserve:
;==============================================================================
DECPROC GN_ControlDDrawDDVersionInfo, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; This Microsoft DDK said to do this. Stuff the run-time
        ; version of DirectX which the DirectX driver was built to
        ; handle in dwHALVersion. If the lpOut ptr it NULL, then fail.
        sub     eax,eax
        cmp     eax,lpOut
        je      Done

        sub     ebx,ebx
        les     bx,lpIn
        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        push    es:[ebx].MYDCICMD.dwParam1
        call    HW_GetDirectXRuntimeVersion

        sub     ebx,ebx
        les     bx,lpOut
        mov     es:[ebx].MYDDVERSIONDATA.dwHALVersion,eax
        mov     eax,1
Done:
CLOSEPROC

;==============================================================================
;
; Function:     GN_RollerToolEscape
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is DCICOMMAND and the subfunction
;               is EDDESCAPE.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      eax     0       failure
;                       non-0   Success
;
; Preserve:
;==============================================================================
DECPROC GN_RollerToolEscape, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; This Microsoft DDK said to do this. Stuff the run-time
        ; version of DirectX which the DirectX driver was built to
        ; handle in dwHALVersion. If the lpOut ptr it NULL, then fail.
        sub     eax,eax
        cmp     eax,lpIn
        je      Done

        sub     ebx,ebx
        les     bx,lpIn
        cmp     es:[ebx].EDDESC_SCROLL_DATA.dwCommand,EDDESC_SCROLL
        jne     Done
        cmp     es:[ebx].EDDESC_SCROLL_DATA.dwSize,size EDDESC_SCROLL_DATA
        jne     Done
        mov     eax,es:[ebx].EDDESC_SCROLL_DATA.cyLines

        add     ds:[esi].GNLOGDEVCX.dwCurrentLine,eax
        jns     @F
        mov     ds:[esi].GNLOGDEVCX.dwCurrentLine,0

@@:
        mov     ecx,ds:[esi].GNLOGDEVCX.pGNBoardCX
        mov     eax,ds:[ecx].GNBOARDCX.dwMemorySizeForValidateMode
        sub     edx,edx
        idiv    ds:[esi].DIBENGINEHDR.deDeltaScan

        ; eax = whole height of videomemory.
        movzx   ecx,ds:[esi].DIBENGINEHDR.deHeight
        mov     edx,ds:[esi].GNLOGDEVCX.dwCurrentLine
        add     edx,ecx
        cmp     edx,eax
        jbe     @F
        sub     eax,ecx
        mov     ds:[esi].GNLOGDEVCX.dwCurrentLine,eax

@@:
        mov     ecx,ds:[esi].DIBENGINEHDR.deDeltaScan
        mov     eax,ds:[esi].GNLOGDEVCX.dwCurrentLine
        imul    eax,ecx

IF 0
        ; Tell the CRTC0 where the base of the panning should be.
        push    dword ptr ds:[esi].GNLOGDEVCX.pGNCrtcCX[0]
        push    eax                                     ;PanBase
        push    ecx                                     ;Pitch
        call    GN_SetPanBaseAndPitch
ENDIF
        mov     eax,1
Done:
CLOSEPROC

;==============================================================================
;
; Function:     GN_MouseTrails
;
; Purpose:      This function is called when an escape is changing the
;               mouse trails state.
;
;               I could not find a definition of the lpIn and lpOut
;               ptrs when wFunction = MOUSETRAILS, but by using the
;               debugger I inferred the following:
;               lpOut is NULL
;               lpIn points to a 16bit value which tells the number
;               of trails to display. It is 0 to turn off mouse trails.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:
;               ecx     = 0 so that we will pass the call to the DIBENG
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_MouseTrails, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; Enumerate CRTCs attached
        mov     dwCRTCCount,0
        lea     edi,ds:[esi].GNLOGDEVCX.pGNCrtcCX

LoopCRTC:
        ; If this CRTC is not a NULL context ...
        mov     edx,ds:[edi]
        or      edx,edx
        je      NextCrtc

        ; Get the cursor cache ptr
        mov     ecx,ds:[edx].GNCRTCCX.pCursorCache
        or      ecx,ecx
        je      NextCrtc

        ; Clear cursor cache
        push    ecx
        push    edx
        call    GN_ClearCursorCache
        pop     ecx

        ; Turn on / off the TRAILS bit accordingly
        sub     ebx,ebx
        les     bx,lpIn

        or      ds:[ecx].CURSORCACHE.dwCursorFlags,CURSOR_HAS_TRAILS OR CURSOR_MOVE
        cmp     word ptr es:[ebx],0
        jne     @F
        and     ds:[ecx].CURSORCACHE.dwCursorFlags,NOT (CURSOR_HAS_TRAILS OR CURSOR_MOVE)

        ; Initialize position for h/w cursor only.
        push    ecx
        mov     eax,dword ptr ds:[ecx].CURSORCACHE.wCursorX
        ror     eax,10H
        push    eax
        call    GENERIC_MoveCursor
        pop     ecx
@@:
        ; Now patch the cursor depended entries
        push    ds:[ecx].CURSORCACHE.dwCursorFlags
        call    GN_TurnExcludeOffOrOn

NextCrtc:
        add     edi,4
        inc     dwCRTCCount
        mov     eax,dwCRTCCount
        cmp     eax,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      LoopCRTC

        ; Return that we want the call passed to the DIBENG also.
        sub     ecx,ecx
CLOSEPROC


;==============================================================================
;
; Function:     GN_SoftwareCursor
;
; Purpose:      This function is called when an escape is changing to/from
;               a hardware cursor from/to a software cursor.
;
;               I could not find a definition of the lpIn and lpOut
;               ptrs when wFunction = MOUSETRAILS, but by using the
;               debugger I inferred the following:
;               lpOut is NULL
;               lpIn points to a 16bit value which tells the number
;               of trails to display. It is 0 to turn off mouse trails.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:
;               ecx     = 0 so that we will pass the call to the DIBENG
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_SoftwareCursor, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; Enumerate CRTCs attached
        mov     dwCRTCCount,0
        lea     edi,ds:[esi].GNLOGDEVCX.pGNCrtcCX

LoopCRTC:
        ; If this CRTC is not a NULL context ...
        mov     edx,ds:[edi]
        or      edx,edx
        je      NextCrtc

        ; Get the cursor cache ptr
        mov     ecx,ds:[edx].GNCRTCCX.pCursorCache
        or      ecx,ecx
        je      NextCrtc

        ; Clear cursor cache
        push    ecx
        push    edx
        call    GN_ClearCursorCache
        pop     ecx

        ; Turn on / off the TRAILS bit accordingly
        sub     ebx,ebx
        les     bx,lpIn

        or      ds:[ecx].CURSORCACHE.dwCursorFlags,CURSOR_IS_SOFTWARE OR CURSOR_MOVE
        cmp     word ptr es:[ebx],0
        jne     @F
        and     ds:[ecx].CURSORCACHE.dwCursorFlags,NOT (CURSOR_IS_SOFTWARE OR CURSOR_MOVE)

        ; Initialize position for h/w cursor only.
        push    ecx
        mov     eax,dword ptr ds:[ecx].CURSORCACHE.wCursorX
        ror     eax,10H
        push    eax
        call    GENERIC_MoveCursor
        pop     ecx
@@:
        ; Now patch the cursor depended entries
        push    ds:[ecx].CURSORCACHE.dwCursorFlags
        call    GN_TurnExcludeOffOrOn

NextCrtc:
        add     edi,4
        inc     dwCRTCCount
        mov     eax,dwCRTCCount
        cmp     eax,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jc      LoopCRTC

        ; Return that we want the call passed to the DIBENG also.
        sub     ecx,ecx
CLOSEPROC


;==============================================================================
;
; Function:     GN_ControlReturnPDevice
;
; Purpose:      This function returns the PDevice as both
;               a 16:16 and a flat 32bit ptr.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:
;               The linear address is provided as the first dword in the
;               lpOut structure and a 16:16 ptr is provided in the 2nd dword.
;               See escape.h for mode details.
;               Aslo return ecx = 1 to show that we handled the call.
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_ControlReturnPDevice, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        sub     edi,edi
        les     di,lpOut
        mov     es:[edi],esi
        mov     eax,cs:[CSlp16LogDevice_In_NVTEXT16]
        mov     es:[edi + 4],eax
CLOSEPROC


;==============================================================================
;
; Function:     GN_SetCplDword
;
; Purpose:      This function saves the dword passed in. It can be
;		gotten again by the caller by issuing the
;		NV_ESC_GET_CPL_DWORD. Note that the display driver
;		makes no attempt to interpret this data in any way.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:
;               None
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_SetCplDword, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        sub     edi,edi
        les     di,lpOut
        mov     eax,es:[edi]
        mov	ds:[esi].GNLOGDEVCX.dwCplData,eax
        mov	eax,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_GetCplDword
;
; Purpose:      This function returns the cpl dword. It was saved
;		when the caller issued the NV_ESC_SET_CPL_DWORD.
;		It was initialized to -1 at boot.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:
;               dword pointed to by lpOut is set to the saved
;		control panel data.
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_GetCplDword, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        sub     edi,edi
        les     di,lpOut
        mov	eax,ds:[esi].GNLOGDEVCX.dwCplData
        mov     es:[edi],eax
        mov	eax,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_ControlAllocSelectorAndMap
;
; Purpose:      This function allocs a selector and maps it to
;               a linear address given for the given length.
;
;               The lpIn argument should point to 2 DWORDS.
;               The first DWORD is the linear base address the
;               newly allcated seelctor should get.
;               The second DWORD specifies the limit of the selector.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:
;               The return value is a 16bit WORD that is placed where
;               lpOut points to. The 16bit value is the newly allocated
;               slector. It is 0 if the routine failed.
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_ControlAllocSelectorAndMap, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        sub     edi,edi
        les     di,lpIn
        mov     eax,1
        mov     ebx,es:[edi]
        mov     ecx,es:[edi + 4]
        call    GN_AllocSelectorAndMapIt
        sub     edi,edi
        les     di,lpOut
        mov     es:[edi],eax
CLOSEPROC

;==============================================================================
;
; Function:     GN_ControlUnmapAndFreeSelector
;
; Purpose:      This function unmaps and frees a selector.
;
;               The first 16 bit WORD pointed to by lpIn has
;               the selector to free.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      None
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_ControlUnmapAndFreeSelector, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        sub     edi,edi
        les     di,lpIn
        movzx   eax,word ptr es:[edi]
        or      eax,eax
        je      @f
        call    GN_UnmapAndFreeSelector
        mov     eax,1
@@:
CLOSEPROC


;==============================================================================
;
; Function:     GN_ControlAllocSharedMemory
;
; Purpose:      This function allocates shared memory for the
;               caller and remembers the allocation.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      None
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_ControlAllocSharedMemory, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        mov     edi,ds:[esi].GNLOGDEVCX.pGNBoardCX

        ; Look for a free entry in the sSharedMemBlocks[] array
        sub     ecx,ecx
@@:
        cmp     ds:[edi].GNBOARDCX.sSharedMemBlocks[ecx * 4].dwData,0
        je      FoundOne
        inc     ecx
        cmp     ecx,MAX_SHARED_MEM_BLOCKS
        jc      @B
        sub     eax,eax
        jmp     DoneIt

FoundOne:
        PUSHR   ecx
        sub     ebx,ebx
        les     bx,lpIn
        mov     eax,es:[ebx].ESC_ALLOC_SHARED_MEMORY_IN.dwNumBytes
        call    GN_MemoryAlloc
        POPR    ecx
        or      eax,eax
        je      DoneIt

        ; Save off the memory allocated in the GNBOARDCX
        sub     ebx,ebx
        les     bx,lpIn
        shl     edx,10H
        mov     ds:[edi].GNBOARDCX.sSharedMemBlocks[ecx * 4].dwData,edx
        mov     eax,es:[ebx].ESC_ALLOC_SHARED_MEMORY_IN.dwID
        mov     ds:[edi].GNBOARDCX.sSharedMemBlocks[ecx * 4].dwID,eax
        shr     edx,10H
        push    edx
        call    GN_GetSelectorBase

DoneIt:
        sub     ebx,ebx
        les     bx,lpOut
        mov     es:[ebx].ESC_ALLOC_SHARED_MEMORY_OUT.pMemBlock,eax
        mov     eax,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_ControlFreeSharedMemory
;
; Purpose:      This function frees shared memory for the caller.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      None
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_ControlFreeSharedMemory, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        mov     edi,ds:[esi].GNLOGDEVCX.pGNBoardCX
        sub     ebx,ebx
        les     bx,lpIn
        mov     eax,es:[ebx].ESC_FREE_SHARED_MEMORY_IN.dwID

        ; Look for this ID in the sSharedMemBlocks[] array
        sub     ecx,ecx
@@:
        cmp     eax,ds:[edi].GNBOARDCX.sSharedMemBlocks[ecx * 4].dwID
        je      FoundOne
        inc     ecx
        cmp     ecx,MAX_SHARED_MEM_BLOCKS
        jc      @B
        sub     eax,eax
        jmp     DoneIt

FoundOne:
        mov     eax,ds:[edi].GNBOARDCX.sSharedMemBlocks[ecx * 4].dwData
        mov     ds:[edi].GNBOARDCX.sSharedMemBlocks[ecx * 4].dwData,0
        shr     eax,10H
        call    GN_MemoryFree
        mov     eax,1

DoneIt:
        sub     ebx,ebx
        les     bx,lpOut
        mov     es:[ebx].ESC_FREE_SHARED_MEMORY_OUT.dwSuccess,eax
        mov     eax,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_ControlGetSharedMemory
;
; Purpose:      This function gets a shared memory ptr allocated
;               at some point in the past.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      None
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_ControlGetSharedMemory, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        mov     edi,ds:[esi].GNLOGDEVCX.pGNBoardCX
        sub     ebx,ebx
        les     bx,lpIn
        mov     edx,es:[ebx].ESC_GET_SHARED_MEMORY_IN.dwID

        ; Look for this ID in the sSharedMemBlocks[] array
        sub     ecx,ecx
@@:
        cmp     edx,ds:[edi].GNBOARDCX.sSharedMemBlocks[ecx * 4].dwID
        je      FoundIt
        inc     ecx
        cmp     ecx,MAX_SHARED_MEM_BLOCKS
        jc      @B
        sub     eax,eax
        jmp     DoneIt

FoundIt:
        mov     eax,ds:[edi].GNBOARDCX.sSharedMemBlocks[ecx * 4].dwData
        shr     eax,10H
        push    eax
        call    GN_GetSelectorBase

DoneIt:
        sub     ebx,ebx
        les     bx,lpOut
        mov     es:[ebx].ESC_GET_SHARED_MEMORY_OUT.pMemBlock,eax
        mov     eax,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_ControlSetCursor
;
; Purpose:      This function sets a cursor.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      None
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_ControlSetCursor, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; We allow applications to directly set the cursor, allowing
        ; cursors that are different sized from what Windows will allow.
        ; Also, other formats are supported for debug builds.
        push    dword ptr lpIn
        call    GENERIC_SetCursor
        mov     eax,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_DvdMacrovisionCommand
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is NV_ESC_DVD_MACROVISION_CMD.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      eax             appropriate macrovision returns
;
; Preserve:
;==============================================================================
DECPROC GN_DvdMacrovisionCommand, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; If this is not the first ID that has been set AND it does not
        ; match the current ID, then there is an error.
        mov     eax,ds:[esi].GNLOGDEVCX.dwMacrovisionNavigatorID
        or      eax,eax
        je      @F
        sub     ebx,ebx
        les     bx,lpIn
        cmp     eax,es:[ebx].MACROVISION_INFO.dwMacrovisionNavigatorID
        je      @F
        les     di,lpOut
        mov     dword ptr es:[ebx].MACROVISION_INFO.dwMacrovisionData,'DABU'
        sub     eax,eax
        jmp     DoneIt
@@:
        ; The ID is OK. If the display device is a real TV or PAL
        ; system, then save it. Otherwise, we don't care.
        push    edi
        call    GN_GetDevData
        movzx   edx,al
        mov     eax,1
        cmp     edx,DEVTYPE_TV
        jne     DoneIt

        cmp     ds:[esi].GNLOGDEVCX.dwMacrovisionNavigatorID,0
        jne     DoneIt
        sub     ebx,ebx
        les     bx,lpIn
        mov     edx,es:[ebx].MACROVISION_INFO.dwMacrovisionNavigatorID
        mov     ds:[esi].GNLOGDEVCX.dwMacrovisionNavigatorID,edx
        sub     eax,eax
DoneIt:
CLOSEPROC


;==============================================================================
;
; Function:     GN_OpenGLGetInfo
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is OPENGL_GETINFO.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:
;               eax                     1
;               If the first dword in the lpIn ptr is GETINFO_DRVNAME
;               then the lpOut ponts to a OPENGL_GETINFO structure.
;               You should fill in the relevant feilds and return eax = 1.
;               If the first dword of lpIn is not GETINFO_DRVNAME
;               or it is, but you can't fill in the releveant fields
;               return eax = 0.
;
; Preserve:
;==============================================================================
DECPROC GN_OpenGLGetInfo, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        sub     edi,edi
        les     di,lpIn
        sub     eax,eax
        cmp     dword ptr es:[edi],OPENGL_GETINFO_DRVNAME
        jne     DoneIt

        movzx   eax,word ptr lpOut+0
        movzx   ecx,word ptr lpOut+2
        lea     eax,[eax].OPENGL_INFO.awch
        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        push    ecx
        push    eax
        call    HW_GetOpenGLRegistryName

        sub     edi,edi
        les     di,lpOut
        mov     es:[edi].OPENGL_INFO.ulVersion,eax
        mov     es:[edi].OPENGL_INFO.ulDriverVersion,ebx
        mov     eax,1
DoneIt:
CLOSEPROC


;==============================================================================
;
; Function:     GN_OpenGLCommand
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is OPENGL_CMD.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      eax                     0
;
; Preserve:
;==============================================================================
DECPROC GN_OpenGLCommand, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        sub     eax,eax
CLOSEPROC


;==============================================================================
;
; Function:     GN_VideoParameters
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is VIDEO_PARAMETERS.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      eax                     0
;
; Preserve:
;==============================================================================
DECPROC GN_VideoParameters, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        sub     edi,edi
        les     di,lpIn
        push    OFFSET VpRet
        cmp     es:[edi].VIDEOPARAMETERS.dwCommand,VP_COMMAND_GET
        je      GN_VpCommandGet
        cmp     es:[edi].VIDEOPARAMETERS.dwCommand,VP_COMMAND_SET
        je      GN_VpCommandSet
        add     sp,@WordSize
        sub     eax,eax
VpRet:
CLOSEPROC


;==============================================================================
;
; Function:     GN_VpCommandGet
;
; Purpose:      This function is called when the escape is
;               VIDEO_PARAMETERS and the specific flavor is
;               VP_COMMAND_GET.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX
;               es:edi          VIDEOPARAMETERS ptr
;
; Returns:      eax     value to return to GDI
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_VpCommandGet, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        sub     ebx,ebx
FindTV:
        mov     eax,ds:[esi].GNLOGDEVCX.pGNCrtcCX[ebx * 4]
        or      eax,eax
        jz      NextCrtc
        push    ebx
        push    eax
        call    GN_GetDevData
        pop     ebx
        mov     es,word ptr lpIn+2
        cmp     al,DEVTYPE_TV
        je      IsTv
NextCrtc:
        inc     ebx
        cmp     ebx,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jb      FindTV
        sub     ebx,ebx
        mov     es:[edi].VIDEOPARAMETERS.dwFlags,VP_FLAGS_TV_MODE OR VP_FLAGS_TV_STANDARD
        mov     es:[edi].VIDEOPARAMETERS.dwTVStandard,VP_TV_STANDARD_WIN_VGA
        mov     es:[edi].VIDEOPARAMETERS.dwAvailableTVStandard,VP_TV_STANDARD_WIN_VGA
        mov     dwCopyStandard,0
        jmp     GotT
IsTv:
        shr     eax,10H
        mov     ecx,VP_TV_STANDARD_NTSC_M
        cmp     al,TVTYPE_NTSCM
        je      FoundTV
        cmp     al,TVTYPE_NTSCJ
        je      FoundTV
        mov     ecx,VP_TV_STANDARD_PAL_B
FoundTV:
        mov     es:[edi].VIDEOPARAMETERS.dwFlags,VP_FLAGS_TV_MODE OR VP_FLAGS_TV_STANDARD
        mov     es:[edi].VIDEOPARAMETERS.dwTVStandard,ecx
        mov     es:[edi].VIDEOPARAMETERS.dwAvailableTVStandard,ecx
        mov     dwCopyStandard,ecx
GotT:
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[ebx * 4]
        mov     eax,ds:[ebx].GNCRTCCX.pHWCrtcCX
        or      eax,eax
        jne     @F
        mov     eax,ds:[ebx].GNCRTCCX.pHWCrtcCXWhenLocked
@@:     mov     pHWCrtcCXOffset,eax

        mov     es:[edi].VIDEOPARAMETERS.dwAvailableModes,VP_MODE_WIN_GRAPHICS

        ; Set the maximum unscaled width and height
        or      es:[edi].VIDEOPARAMETERS.dwFlags,VP_FLAGS_MAX_UNSCALED
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.deWidth
        movzx   ecx,ds:[esi].GNLOGDEVCX.Dibeng.deHeight
        mov     es:[edi].VIDEOPARAMETERS.dwMaxUnscaledX,eax
        mov     es:[edi].VIDEOPARAMETERS.dwMaxUnscaledY,ecx
        or      es:[edi].VIDEOPARAMETERS.dwFlags,VP_FLAGS_POSITION
        mov     es:[edi].VIDEOPARAMETERS.dwPositionX,0
        mov     es:[edi].VIDEOPARAMETERS.dwPositionY,0
        mov     es:[edi].VIDEOPARAMETERS.dwCPType,0
        mov     es:[edi].VIDEOPARAMETERS.dwCPStandard,0

        push    pHWCrtcCXOffset
        call    HW_IsMacroVisionEncoderAttached
        or      eax,eax
        je      No_MV

        ; Yes, we have macrovision
        or      es:[edi].VIDEOPARAMETERS.dwFlags,VP_FLAGS_COPYPROTECT
        mov     es:[edi].VIDEOPARAMETERS.dwCPType,VP_CP_TYPE_APS_TRIGGER
        mov     eax,ds:[esi].GNLOGDEVCX.dwMacrovisionMode
        mov     es:[edi].VIDEOPARAMETERS.bCP_APSTriggerBits,eax
        mov     eax,dwCopyStandard
        mov     es:[edi].VIDEOPARAMETERS.dwCPStandard,eax
No_MV:
        mov     eax,1
CLOSEPROC


;==============================================================================
;
; Function:     GN_VpCommandSet
;
; Purpose:      This function is called when the escape is
;               VIDEO_PARAMETERS and the specific flavor is
;               VP_COMMAND_SET.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX
;               es:edi          VIDEOPARAMETERS ptr
;
; Returns:      eax     value to return to GDI
;
; Preserve:     Nothing
;==============================================================================
DECPROC GN_VpCommandSet, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC

        ; The only "set" command we support is CopyProtect
        test    es:[edi].VIDEOPARAMETERS.dwFlags,VP_FLAGS_COPYPROTECT
        jz      Fail

        sub     ebx,ebx

        ; We'll be using these local variables.
        ; dwCRTCCount = last non-TV device index.
        ; dwCopyStandard = 1 if we are forcing Macrovision for a non-TV device.
        mov     dwCRTCCount,ebx
        mov     dwCopyStandard,ebx

FindTV1:
        mov     eax,ds:[esi].GNLOGDEVCX.pGNCrtcCX[ebx * 4]
        or      eax,eax
        jz      NextCrtc1
        mov     dwCRTCCount,ebx
        push    ebx
        push    eax
        call    GN_GetDevData
        pop     ebx
        cmp     al,DEVTYPE_TV
        je      FoundTv
NextCrtc1:
        inc     ebx
        cmp     ebx,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jb      FindTV1

        ; If no TV device was found, use the last non-TV device to
        ; make sure the new Macrovision level gets down to the RM.
        mov     ebx,dwCRTCCount
        mov     eax,ds:[esi].GNLOGDEVCX.pGNCrtcCX[ebx * 4]
        or      eax,eax
        jz      SkipToFail
        push    ebx
        push    eax
        call    GN_GetDevData
        pop     ebx
                mov     dwCopyStandard,1
        jmp     FoundTv

SkipToFail:
        jmp     Fail

FoundTv:
        mov     es,word ptr lpIn+2
        mov     ebx,ds:[esi].GNLOGDEVCX.pGNCrtcCX[ebx * 4]
        mov     pGNCrtcCXOffset,ebx
        mov     eax,ds:[ebx].GNCRTCCX.pHWCrtcCX
        or      eax,eax
        jne     @F
        mov     eax,ds:[ebx].GNCRTCCX.pHWCrtcCXWhenLocked
@@:     mov     pHWCrtcCXOffset,eax

        ; Ask hardware if we have macrovision capability
        push    pHWCrtcCXOffset
        call    HW_IsMacroVisionEncoderAttached
        mov     ebx,pGNCrtcCXOffset
        or      eax,eax
        je      Fail

        ; The only copy protection mechanism we support is APS trigger
        cmp     es:[edi].VIDEOPARAMETERS.dwCPType,VP_CP_TYPE_APS_TRIGGER
        jne     Fail

        ; Are we being asked to activate copy protection?
        cmp     es:[edi].VIDEOPARAMETERS.dwCPCommand,VP_CP_CMD_ACTIVATE
        jne     Next1

        ; Try to set the Copy protection
        mov     eax,es:[edi].VIDEOPARAMETERS.bCP_APSTriggerBits
        and     eax,3
        mov     ecx,eax
        xchg    ds:[esi].GNLOGDEVCX.dwMacrovisionMode,eax
        or      eax,eax
        jnz     @F
        or      ecx,4
@@:

        push    ecx

        ; If we are forcing Macrovision through a non-TV device to
        ; get the Macrovision level to the RM, use default width/height.
        cmp     dwCopyStandard, 1
        jnz     SkipNormal
        mov     eax,800
        push    eax
        mov     eax,600
        push    eax
        jmp     SkipCommon

        ; Else use normal calculated width and height.
SkipNormal:
        mov     eax,ds:[ebx].GNCRTCCX.rcPhysicalViewport.dwRight
        sub     eax,ds:[ebx].GNCRTCCX.rcPhysicalViewport.dwLeft
        push    eax
        mov     eax,ds:[ebx].GNCRTCCX.rcPhysicalViewport.dwBottom
        sub     eax,ds:[ebx].GNCRTCCX.rcPhysicalViewport.dwTop
        push    eax

SkipCommon:
        push    pHWCrtcCXOffset
        call    HW_SetMacrovisionMode
        or      eax,eax
        je      Fail

        ; Save current key and bump to next key
        mov     eax,ds:[esi].GNLOGDEVCX.dwNextMacrovisionKey
        mov     ds:[esi].GNLOGDEVCX.dwCurrentMacrovisionKey,eax
        mov     es:[edi].VIDEOPARAMETERS.dwCPKey,eax
        inc     ds:[esi].GNLOGDEVCX.dwNextMacrovisionKey
        mov     eax,1
        jmp     Done
Next1:
        cmp     es:[edi].VIDEOPARAMETERS.dwCPCommand,VP_CP_CMD_DEACTIVATE
        jne     Next2

        ; We can deactivate if the requester has the right key
        mov     eax,DISP_CHANGE_BADPARAM
        mov     edx,ds:[esi].GNLOGDEVCX.dwCurrentMacrovisionKey
        cmp     edx,es:[edi].VIDEOPARAMETERS.dwCPKey
        jne     Done
        sub     eax,eax
        mov     ecx,eax
        xchg    ds:[esi].GNLOGDEVCX.dwMacrovisionMode,eax
        or      eax,eax
        jz      @F
        mov     ecx,4
@@:
        push    ecx
        mov     eax,ds:[ebx].GNCRTCCX.rcPhysicalViewport.dwRight
        sub     eax,ds:[ebx].GNCRTCCX.rcPhysicalViewport.dwLeft
        push    eax
        mov     eax,ds:[ebx].GNCRTCCX.rcPhysicalViewport.dwBottom
        sub     eax,ds:[ebx].GNCRTCCX.rcPhysicalViewport.dwTop
        push    eax
        push    pHWCrtcCXOffset
        call    HW_SetMacrovisionMode
        jmp     Done
Next2:
        cmp     es:[edi].VIDEOPARAMETERS.dwCPCommand,VP_CP_CMD_CHANGE
        jne     Next3

        ; We can change the CP if the requester has the right key
        mov     eax,DISP_CHANGE_BADPARAM
        mov     edx,ds:[esi].GNLOGDEVCX.dwCurrentMacrovisionKey
        cmp     edx,es:[edi].VIDEOPARAMETERS.dwCPKey
        jne     Done
        mov     eax,es:[edi].VIDEOPARAMETERS.bCP_APSTriggerBits
        and     eax,3
        mov     ecx,eax
        xchg    ds:[esi].GNLOGDEVCX.dwMacrovisionMode,eax
        or      eax,eax
        jz      WasNotProt
        or      ecx,ecx
        jnz     ChangeProt
ChangeBrightness:
        or      ecx,4
        jmp     ChangeProt
WasNotProt:
        or      ecx,ecx
        jnz     ChangeBrightness
ChangeProt:
        push    ecx
        mov     eax,ds:[ebx].GNCRTCCX.rcPhysicalViewport.dwRight
        sub     eax,ds:[ebx].GNCRTCCX.rcPhysicalViewport.dwLeft
        push    eax
        mov     eax,ds:[ebx].GNCRTCCX.rcPhysicalViewport.dwBottom
        sub     eax,ds:[ebx].GNCRTCCX.rcPhysicalViewport.dwTop
        push    eax
        push    pHWCrtcCXOffset
        call    HW_SetMacrovisionMode
        jmp     Done
Next3:
Fail:
        sub     eax,eax
Done:
CLOSEPROC


;==============================================================================
;
; Function:     GN_GetLocalRegistryPath
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is NV_ESC_GET_LOCAL_REGISTRY_PATH.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      eax             1
;
; Preserve:
;==============================================================================
DECPROC GN_GetLocalRegistryPath, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; Load up the logical context
        mov     esi,ds:[esi].GNLOGDEVCX.pGNBoardCX

        sub     edi,edi
        les     di,lpOut
        mov     ecx,4
        mov     eax,dword ptr ds:[esi].GNBOARDCX.szLocalDisplayBasePath
        mov     es:[edi],eax
@@:
        mov     al,ds:[esi].GNBOARDCX.szLocalDisplayBasePath[ecx]
        mov     es:es:[edi][ecx],al
        inc     ecx
        or      al,al
        jne     @B

        ; Tack on NVidia to the end of it
        mov     dword ptr es:[edi][ecx - 1],'ivN\'
        mov     dword ptr es:[edi][ecx + 3],'aid'
        mov     eax,1
CLOSEPROC



;==============================================================================
;
; Function:     GN_GetRegistryModeList
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is NV_ESC_GET_REGISTRY_MODE_LIST.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      eax             1
;
; Preserve:
;==============================================================================
DECPROC GN_GetRegistryModeList, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; Find an available GNCRTCCX ptr, just so we can use
        ; the DISPDATA structure from it
        mov     edx,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        lea     eax,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        sub     eax,4
        sub     ecx,ecx

@@:     add     eax,4
        cmp     ecx,ds:[eax]
        jne     @F
        dec     edx
        jne     @B
        sub	eax,eax
        jmp	LastPart
@@:
        ; If lpIn is NULL, we want to return the number of modes,
        ; which means that we should pass in a NULL ptr for the modelist.
        mov     edx,MODETYPE_MODEENTRY
        sub     ebx,ebx
        cmp     lpIn,0
        je      @F

        ; Check lpIn
        mov     edx,MODETYPE_MODEENTRYNORR
        mov     ebx,lpOut
        les     di,lpIn
        cmp     dword ptr es:[di],MODETYPE_MODEENTRYANDPITCH
        ja      @F
        mov     edx,es:[di]
@@:
        mov     eax,ds:[eax]
        push    ds:[eax].GNCRTCCX.lpNvmodeDispData
        push    ebx
        push    edx
        CALLFAR16IND lpfnGetModeList,es,ax

LastPart:
        ; If lpIn was NULL, then we want to return the number of modes
        cmp     lpIn,0
        jne     @F
        les     di,lpOut
        movzx   eax,ax
        mov     es:[di],eax
@@:
        mov     eax,1
CLOSEPROC

;==============================================================================
;
; Function:     GN_GetPrimaryInfo
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is NV_ESC_PRIMARY_INFO.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      eax             1
;
; Preserve:
;==============================================================================
DECPROC GN_GetPrimaryInfo, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        les     di,lpIn
        mov     eax,es:[di].NV_PRIMARY_INFO.ulHeadNum
        cmp     eax,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jae     FailIt

        ; Find the GNLOGDEVCX that is using this head number
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    eax
        call    GN_MapHeadToGNLogdev
        or      eax,eax
        je      FailIt
        mov     esi,ebx

Success:
        les     di,lpOut
        mov     es:[di].NV_PRIMARY_INFO.ulReturnCode,1
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.deWidth
        mov     es:[di].NV_PRIMARY_INFO.ulWidth2,eax
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.deHeight
        mov     es:[di].NV_PRIMARY_INFO.ulHeight,eax
        movzx   eax,ds:[esi].GNLOGDEVCX.Dibeng.deBitsPixel
        mov     es:[di].NV_PRIMARY_INFO.ulDepth,eax
        mov     eax,ds:[esi].GNLOGDEVCX.Dibeng.deDeltaScan
        mov     es:[di].NV_PRIMARY_INFO.ulPitch,eax
        mov     eax,ds:[esi].GNLOGDEVCX.Dibeng.deBitsOffset
        mov     es:[di].NV_PRIMARY_INFO.ulPrimarydwOffset,eax
        mov     eax,ds:[esi].GNLOGDEVCX.dwNumCRTCsAttachedToThisLogicalDevice
        mov     es:[di].NV_PRIMARY_INFO.ulNumActiveDACs,eax
        sub     eax,eax
        mov     es:[di].NV_PRIMARY_INFO.ulPanningPtr,eax
        mov     es:[di].NV_PRIMARY_INFO.ulFullscreenPtr,eax
        jmp     DoneIt

FailIt:
        ; Clear out everything to zero
        les     di,lpOut
        sub     eax,eax
        mov     es:[di].NV_PRIMARY_INFO.ulReturnCode,eax
        mov     es:[di].NV_PRIMARY_INFO.ulWidth2,eax
        mov     es:[di].NV_PRIMARY_INFO.ulHeight,eax
        mov     es:[di].NV_PRIMARY_INFO.ulDepth,eax
        mov     es:[di].NV_PRIMARY_INFO.ulPitch,eax
        mov     es:[di].NV_PRIMARY_INFO.ulPrimarydwOffset,eax
        mov     es:[di].NV_PRIMARY_INFO.ulPanningPtr,eax
        mov     es:[di].NV_PRIMARY_INFO.ulFullscreenPtr,eax

DoneIt:
        mov     eax,1
CLOSEPROC

;==============================================================================
;
; Function:     GN_GetRegistryDesktopModeList
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is NV_ESC_GET_REGISTRY_DESKTOP_MODE_LIST.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      eax             1
;
; Preserve:
;==============================================================================
DECPROC GN_GetRegistryDesktopModeList, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        ; Find an available GNCRTCCX ptr, just so we can use
        ; the DISPDATA structure from it
        lea     eax,ds:[esi].GNLOGDEVCX.pGNCrtcCX
        mov     edx,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        sub     eax,4
        sub     ecx,ecx

@@:     add     eax,4
        cmp     ecx,ds:[eax]
        jne     @F
        dec     edx
        jne     @B
        sub	eax,eax
        jmp	LastPart
@@:
        ; If lpIn is NULL, we want to return the number of modes,
        ; which means that we should pass in a NULL ptr for the modelist.
        mov     edx,MODETYPE_MODEENTRY
        sub     ebx,ebx
        cmp     lpIn,0
        je      @F

        ; Check lpIn
        mov     edx,MODETYPE_MODEENTRYNORR
        mov     ebx,lpOut
        les     di,lpIn
        cmp     dword ptr es:[di],MODETYPE_MODEENTRYANDPITCH
        ja      @F
        mov     edx,es:[di]
@@:
        mov     eax,ds:[eax]
        push    ds:[eax].GNCRTCCX.lpNvmodeDispData
        push    ebx
        push    edx
        CALLFAR16IND lpfnGetDesktopModeList,es,ax

LastPart:
        ; If lpIn was NULL, then we want to return the number of modes
        cmp     lpIn,0
        jne     @F
        les     di,lpOut
        movzx   eax,ax
        mov     es:[di],eax
@@:
        mov     eax,1
CLOSEPROC

;==============================================================================
;
; Function:     GN_QueryHeadRefreshRate
;
; Purpose:      This function gets called from GENERIC_Control when
;               the escape value is ESC_NV_QUERY_HEAD_REFRESH_RATE.
;
; Arguments:    Control Display Driver Stack Frame
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      eax             1
;
; Preserve:
;==============================================================================
DECPROC GN_QueryHeadRefreshRate, PASCAL, ShareFrameFar16, NEAR
CONTROL_FRAME_AND_LOCALS
OPENPROC
        les     di,lpIn
        mov     eax,es:[di]                     ;Head number
        cmp     eax,ds:[esi].GNLOGDEVCX.dwTotalCRTCsOnBoard
        jae     FailIt

        ; Find the GNLOGDEVCX that is using this head number
        push    ds:[esi].GNLOGDEVCX.pGNBoardCX
        push    eax
        call    GN_MapHeadToGNLogdev
        or      eax,eax
        je      FailIt
        mov     esi,eax

        lea     esi,ds:[esi].GNCRTCCX.sCrtcModeInfo
        mov     eax,ds:[esi].MODEINFO.dwRefreshRate
        les     di,lpOut
        mov     es:[di],eax
        jmp     DoneIt

FailIt:
        les     di,lpOut
        sub     eax,eax
        mov     es:[di],eax

DoneIt:
        mov     eax,1
CLOSEPROC

CLOSESEG _NVTEXT16

END

