;==============================================================================
;
; Copyright (C) 1998, Nvidia Coporation
;
; File:         gnrepnt.asm
;
; Purpose:      This file holds the UserRepaint display driver entry point.
;==============================================================================
.586
incDrawMode = 1
incDevice   = 1
incLogical  = 1
include macros.dat
include gdidefs.inc
include dibeng.inc
include gngdi.inc
include hwgdi.inc
include sysdata.inc
include gndata.inc
include gnlogdev.inc
include gnboard.inc
include gncrtc.inc

USER_REPAINT_ORDINAL    EQU     275
MAX_USER_REPAINT_CALLS  EQU     5
GRAPHICS_IDLE_TIME      EQU     200H

OPENSEG  _DATA, USE16, DATA
; This must be in a DATA segment or else Kernel chokes on it.
PUBLIC  szUserModuleName
szUserModuleName        DB      'USER',0
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE
PUBLIC	lpfnUserRepaint
lpfnUserRepaint	DD	0

IFDEF DEBUG
szIssueRepnt    DB      'DISPLAY:GN_IssueUserRepaint',0DH,0AH,0
ENDIF

;==============================================================================
;
; Function:     GN_InitUserRepaint ()
;
; Purpose:      This function inits the ptr to the GDI function
;               UserRepaint which forces all Windows on the screen
;               to redraw themselves (places a WM_PAINT message in
;               everyones queue for every Window rectangle).
;
; Arguments:	None
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_InitUserRepaint, PASCAL, NOFRAME, FAR16
OPENPROC
        PUSHR   ds
        mov     ax,_DATA
        shl     eax,10H
        mov     ax,OFFSET szUserModuleName
        push    eax
        call    GetModuleHandle

        mov     edx,USER_REPAINT_ORDINAL
        push    ax
        push    edx
        call    GetProcAddress
        shl     edx,10H
        mov     dx,ax
        mov	ds,wNVTEXT16_Alias_In_NVTEXT16
        mov	ds:[lpfnUserRepaint],edx
        POPR    ds
CLOSEPROC


;==============================================================================
;
; Function:     GENERIC_UserRepaintDisable
;
; Purpose:      This function is the gdi bitblt entry point.
;
; Arguments:    UserRepaintDisable display driver stack FRAME
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_UserRepaintDisable, PASCAL, FRAME, FAR16
PARMB   argRepaintDisable
OPENPROC
        PUSHR   ds,esi
        mov     ds,cs:[wFlatDataSel_In_NVTEXT16]
        mov     esi,cs:[CSpGNLogicalDevice_In_NVTEXT16]

        ; If we haven't initialized yet, then do so now
        cmp	lpfnUserRepaint,0
        jnz     @F
        call    GN_InitUserRepaint
@@:
        mov     al,argRepaintDisable
        mov     ds:[esi].GNLOGDEVCX.bRepaintDisable,al
        or      al,al
        jne     @F

        ; We can call UserRepaint -- is there one pending?
        cmp     al,ds:[esi].GNLOGDEVCX.bRepaintPending
        je      @F
        mov     ds:[esi].GNLOGDEVCX.bRepaintPending,al
        push	esi
        call    GN_IssueUserRepaint
@@:
        POPR    ds,esi
CLOSEPROC


;==============================================================================
;
; Function:     GN_UserRepaint
;
; Purpose:      This function can be called to transfer a disabled
;               repaint request into a pending request.
;
; Arguments:    ds			CX Selector
;		pGNLogdevCXOffset	GNLOGDEVCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_UserRepaint, PASCAL, FRAME, FAR16
PARMD	pGNLogdevCXOffset
OPENPROC
        PUSHR   ds,esi
        mov     esi,pGNLogdevCXOffset

        ; If we haven't initialized yet, then do so now
        cmp	lpfnUserRepaint,0
        jnz     @F
        call    GN_InitUserRepaint
@@:
        mov     al,ds:[esi].GNLOGDEVCX.bRepaintDisable
        mov     ds:[esi].GNLOGDEVCX.bRepaintPending,al
        or      al,al
        jne     @F
        push	esi
        call    GN_IssueUserRepaint
@@:
        POPR    ds,esi
CLOSEPROC


;==============================================================================
;
; Function:     GN_IssueUserRepaint
;
; Purpose:      Occassionally there is a problem (Windows OS problem --
;               not our's) with toggling back from a DOS box. The issue
;               is that sometimes our request to USER to repaint the
;               screen is discarded. Hence, the Windows screen is
;               black and gets repainted bit by bit as the user clicks
;               on different pieces of it. The solution is to issue the
;               USER repaint request again if the first request hasn't
;               caused a good amount of graphics activity within a
;               reasonable amount of time.
;
; Arguments:
;		ds			Context Selector
;		pGNLogdevCXOffset	GNLOGDEVCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_IssueUserRepaint, PASCAL, FRAME, NEAR
PARMD	pGNLogdevCXOffset
OPENPROC
        PUSHR   ds,esi

IFDEF DEBUG
        mov     ax,cs
        shl     eax,10H
        lea     ax,szIssueRepnt
        push    eax
        call    GENERIC_OutputDebugString
ENDIF

        mov	esi,pGNLogdevCXOffset

        ; Set a flag saying a repaint has been issued
        mov     ds:[esi].GNLOGDEVCX.bRepaintIssued,1

        ; Call the hardware specific code to get a cookie
        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        call    HW_GetGraphicsTimeStamp
        mov     ds:[esi].GNLOGDEVCX.dwGraphicsTimeStamp,eax

        ; Now issue a repaint
        call    FFP16 PTR lpfnUserRepaint
        POPR    ds,esi
CLOSEPROC


;==============================================================================
;
; Function:     GN_CheckUserRepaint
;
; Purpose:      Occassionally there is a problem (Windows OS problem --
;               not our's) with toggling back from a DOS box. The issue
;               is that sometimes our request to USER to repaint the
;               screen is discarded. Hence, the Windows screen is
;               black and gets repainted bit by bit as the user clicks
;               on different pieces of it. The solution is to issue the
;               USER repaint request again if the first request hasn't
;               caused a good amount of graphics activity within a
;               reasonable amount of time.
;
; Arguments:
;               ds              Context Selector
;               esi             GNLOGDEVCX ptr
;
; Returns:      None
;
; Preserve:     everything!
;==============================================================================
DECPROC GN_CheckUserRepaint, PASCAL, NOFRAME, FAR16
OPENPROC
        cmp     ds:[esi].GNLOGDEVCX.bRepaintIssued,0
        je      DoneIt
        cmp     ds:[esi].GNLOGDEVCX.dwHardwareBusy,0
        jne	DoneIt
        cmp     ds:[esi].GNLOGDEVCX.bDirectXActive,0
        jne	DoneIt
        mov     ds:[esi].GNLOGDEVCX.bRepaintIssued,0

        ; Has alot of graphics activity occurred since then?
        ; Call the hardware specific code to get a cookie
        PUSHR   alld,ds,es,fs,gs

        ; Get the current time stamp
        push    ds:[esi].GNLOGDEVCX.pHWLogdevCX
        call    HW_GetGraphicsTimeStamp

        ; Has it changed much?
        sub     eax,ds:[esi].GNLOGDEVCX.dwGraphicsTimeStamp
        cmp     eax,GRAPHICS_IDLE_TIME
        ja      NoMoreRepaints
	cmp	ds:[esi].GNLOGDEVCX.bUserRepaintCallCount,MAX_USER_REPAINT_CALLS
	ja	NoMoreRepaints

        ; Not much graphics activity has happened. Better issue
        ; another repaint.
        push	esi
        call    GN_IssueUserRepaint
	inc	ds:[esi].GNLOGDEVCX.bUserRepaintCallCount
	jmp	RepaintIssued

NoMoreRepaints:
	mov	ds:[esi].GNLOGDEVCX.bUserRepaintCallCount,0

RepaintIssued:
        POPR    alld,ds,es,fs,gs
DoneIt:
CLOSEPROC

CLOSESEG _NVTEXT16

END

