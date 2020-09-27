;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnint2f.asm
;
; Purpose:      This file holds the routines dealing with int2f.
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

SCREEN_SWITCH_OUT       EQU     4001H
SCREEN_SWITCH_IN        EQU     4002H

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE
PUBLIC  lpfnOldInt2FHandler
lpfnOldInt2FHandler     DD      0
dwInt2FInstalled        DD      0

PUBLIC	lpUserFullscreenDOSBoxFlag
lpUserFullscreenDOSBoxFlag	DD	0

IFDEF DEBUG
szToWindows     DB      'DISPLAY:GN_Int2FSwitchingToWindows',0DH,0AH,0
szToDos         DB      'DISPLAY:Int2FSwitchingToDos',0DH,0AH,0
ENDIF

;==============================================================================
;
; Function:     GN_HookInt2F
;
; Purpose:      This function hooks into the int2f chain if this is
;               the primary device and we have not already hooked
;               the chain. It is a NOP if we are a secondary
;               device or we are already in the chain.
;
; Arguments:
;               ds              Context Selector
;               pLogdevCXOffset GNLOGDEVCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_HookInt2F, PASCAL, FRAME, FAR16
PARMD   pLogdevCXOffset
OPENPROC
        PUSHR   ds
        sub     eax,eax

        ; Have we already hooked it?
        cmp     eax,cs:[dwInt2FInstalled]
        jne     done

        ; Are we the primary device?
        cmp     cs:[dwThisLogicalDeviceHandle_In_NVTEXT16],1
        jne     done

        ; Use the DOS int 21H interface to get and set the 2F vector
        cli
        mov     ax,3500H+2FH
        int     21H
        mov     ds,cs:[wNVTEXT16_Alias_In_NVTEXT16]
        mov     word ptr ds:[lpfnOldInt2FHandler + 0],bx
        mov     word ptr ds:[lpfnOldInt2FHandler + 2],es
        mov     ds:[dwInt2FInstalled],1

        ; Now set it to our interrupt handler
        mov     ax,SEG Int2FHandler
        mov     edx,OFFSET Int2FHandler
        mov     ds,ax
        mov     ax,2500H + 2FH
        int     21H
        sti

	push    pLogdevCXOffset
	call	GN_GetUserDosBoxFlagAddress

        ; We must tell the main VDD that we;d like to recieve int2fs.
        mov     eax,4000H
        int     2FH
done:
        POPR    ds
CLOSEPROC


;==============================================================================
;
; Function:     GN_UnhookInt2F
;
; Purpose:      This function unhooks us from the int2f chain if
;               we are currently hooked. Otherwise it just returns.
;
; Arguments:
;               ds              Context Selector
;               pLogdevCXOffset GNLOGDEVCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_UnhookInt2F, PASCAL, FRAME, FAR16
PARMD   pLogdevCXOffset
OPENPROC
        PUSHR   ds

        sub     eax,eax
        cmp     eax,cs:[dwInt2FInstalled]
        je      done

        ; Use the DOS int 21H interface to reset the old vector
        cli
        sub     ecx,ecx
        mov     eax,cs:[lpfnOldInt2FHandler]
        mov     ds,cs:[wNVTEXT16_Alias_In_NVTEXT16]
        mov     ds:[dwInt2FInstalled],ecx
        mov     dx,ax
        shr     eax,10H
        mov     ds,ax
        mov     ax,2500H + 2FH
        int     21H
        sti
done:
       POPR     ds
CLOSEPROC


;==============================================================================
;
; Function:     Int2FHandler
;
; Purpose:      This function is the int2F handler. Lots of things come
;               through here, but we only care about ScreenSwitchIn and
;               ScrenSwitchOut.
;
; Arguments:    None
;
; Returns:      None
;
; Preserve:     All registers must be preserved
;==============================================================================
DECPROC Int2FHandler, PASCAL, NOFRAME, FAR16
OPENPROC
        cmp     ax,SCREEN_SWITCH_IN
        je      Int2FSwitchingToWindows
        cmp     ax,SCREEN_SWITCH_OUT
        je      Int2FSwitchingToDos
        jmp     FFP16 PTR lpfnOldInt2FHandler

Int2FHandler    ENDP


;==============================================================================
;
; Function:     Int2FSwitchingToWindows
;
; Purpose:      This function is the int2F handler once it is known
;               that we are doing a screen switch in (going to Windows).
;
; Arguments:    None
;
; Returns:      None
;
; Preserve:     All registers must be preserved
;==============================================================================
DECPROC Int2FSwitchingToWindows, PASCAL, NOFRAME, FAR16
OPENPROC
        push    bp
        mov     bp,sp
        PUSHR   alld,ds,es,fs,gs
        call    GN_Int2FSwitchingToWindows
        POPR    alld,ds,es,fs,gs
        and     word ptr [bp + 6],NOT 1
        pop     bp
        iret

Int2FSwitchingToWindows ENDP


;==============================================================================
;
; Function:     GN_Int2FSwitchingToWindows
;
; Purpose:      This function is the same as GN_Int2FSwitchingToWindows
;               and is called from it.  The only difference is, this
;               routine can be called from anywhere because it does a
;               ret instead of an iret.
;
; Arguments:    None
;
; Returns:      None
;
; Preserve:     ds, esi, edi
;==============================================================================
DECPROC GN_Int2FSwitchingToWindows, PASCAL, NOFRAME, FAR16
OPENPROC
        PUSHR   ds,esi,edi

IFDEF DEBUG
        mov     ax,cs
        shl     eax,10H
        lea     ax,szToWindows
        push    eax
        call    GENERIC_OutputDebugString
ENDIF
        mov     ds,cs:[wFlatDataSel_In_NVTEXT16]
        mov     esi,cs:[CSpGNLogicalDevice_In_NVTEXT16]
        mov     ds:[esi].GNLOGDEVCX.bSetModeBypass,0

        ; Bypass routine if we are already in windows mode.
        cmp     ds:[esi].GNLOGDEVCX.dwInt2FScreenSwitchOut,0
        je      @F

        ; Send a message down the chain
        push    esi
        pushd   MSG_DOSOFF
        push    esi                             ; CX ptr to DIBENGINEHDR
        push	CSlp16LogDevice_In_NVTEXT16	; 16:16 ptr to DIBENGINEHDR
        call    GN_LogdevMessage

        ; Tell the VDD we are going to HiRes
        call    GN_VddDriverRegister

        ; UserRepaint sometimes takes a few seconds to take effect.
        ; Clear the screen to black or else we see garbage on the
        ; screen for those few seconds.
        sub     eax,eax
        push    CSlp16LogDevice_In_NVTEXT16
        push    eax
        push    eax
        push    eax
        push    ds:[esi].DIBENGINE.deWidth
        push    ds:[esi].DIBENGINE.deHeight
        push    eax
        push    eax
        push    eax
        call    GENERIC_BitBlt

        ; Now ask User to repaint the screen
        push	esi
        call    GN_UserRepaint

	cmp	cs:[lpUserFullscreenDOSBoxFlag],0
	je	@F
	; This corrects for the fact that occassionally USER does
	; not RESET its own internal flag that indicates that it is
	; no longer in a fullscreen DOS box.
	lds	si,cs:[lpUserFullscreenDOSBoxFlag]
	and	byte ptr ds:[si],NOT 8
@@:
        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     Int2FSwitchingToDos
;
; Purpose:      This function is the int2F handler once it is known
;               that we are doing a screen switch out (going to Dos).
;
; Arguments:    None
;
; Returns:      None
;
; Preserve:     All registers must be preserved
;==============================================================================
DECPROC Int2FSwitchingToDos, PASCAL, NOFRAME, FAR16
OPENPROC
        push    bp
        mov     bp,sp
        PUSHR   alld,ds,es,fs,gs

IFDEF DEBUG
        mov     ax,cs
        shl     eax,10H
        lea     ax,szToDos
        push    eax
        call    GENERIC_OutputDebugString
ENDIF

        mov     ds,cs:[wFlatDataSel_In_NVTEXT16]
        mov     esi,cs:[CSpGNLogicalDevice_In_NVTEXT16]

        ; Clear the screen before issuing the unregister. Otherwise
        ; we sometimes see a flash of garbage.
        sub     eax,eax
        push    CSlp16LogDevice_In_NVTEXT16
        push    eax
        push    eax
        push    eax
        push    ds:[esi].DIBENGINE.deWidth
        push    ds:[esi].DIBENGINE.deHeight
        push    eax
        push    eax
        push    eax
        call    GENERIC_BitBlt

        ; If the BUSY bit is already set, it is beacuse the VDD is
        ; orchestrating the switch to DOS (e.g. user pressed Alt-Tab)
        ; If it is not set, then inform the VDD we are going to DOS.
        test    ds:[esi].GNLOGDEVCX.Dibeng.deFlags,BUSY
        jnz     @F

        call    GN_VddDriverUnregister
@@:
        ; Send a message down the chain
        push    esi
        pushd   MSG_DOSON
        push    esi                             ; CX ptr to DIBENGINEHDR
        push	CSlp16LogDevice_In_NVTEXT16	; 16:16 ptr to DIBENGINEHDR
        call    GN_LogdevMessage

        POPR    alld,ds,es,fs,gs
        and     word ptr [bp + 6],NOT 1
        pop     bp
        iret

Int2FSwitchingToDos     ENDP


;==============================================================================
;
; Function:     GENERIC_ResetHiresMode
;
; Purpose:      This function is called from the main VDD when switching
;               from a full screen DOS back to Windows. We need to get
;               back into a HiRes mode here. (The resource manager
;               may be catching the SET_FOCUS call in VDD land and
;               doing the mode set itself, so we may not need to do
;               anything here. Just in case.
;
; Arguments:    None
;
; Returns:      None
;
; Preserve:     All registers must be preserved
;==============================================================================
DECPROC GENERIC_ResetHiresMode, PASCAL, NOFRAME, FAR16
OPENPROC
        PUSHR   ds,esi,edi

        mov     ds,cs:[wFlatDataSel_In_NVTEXT16]
        mov     esi,cs:[CSpGNLogicalDevice_In_NVTEXT16]

        mov     ds:[esi].GNLOGDEVCX.dwInt2FScreenSwitchOut,1
        call    GN_Int2FSwitchingToWindows

        POPR    ds,esi,edi
CLOSEPROC


;==============================================================================
;
; Function:	GN_GetUserDosBoxFlagAddress
;
; Purpose:
;		This fixes a bug in USER that is DOS-box switching timing
;		related. The probem is that USER maintains a bit, which
;		when set, indicates we are in a full-screen DOS box; when
;		clear, we are in the Windows desktop. Sometimes, when
;		coming back from a fullscreen DOS box, particularly on
; 		the character mode tests in WHQL, for some reason, the
;		bit does not get cleared and USER ends up thinking that
;		we are in a full-screen DOS BOX when we are really on
;		the Windows desktop. This causes ChangeDisplaySettingsEx
;		to fail without even getting to the display driver.
;
;		The solution is to flip this bit ourselves whenever we
;		receive a SET_DEVICE_FOCUS message in the minivdd to
;		switch from FullScreenDos to FullScreen Windows.
;
; Arguments:	pLogdevCXOffset		GNLOGDEVCX ptr
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GN_GetUserDosBoxFlagAddress, PASCAL, FRAME, FAR16
PARMD   pLogdevCXOffset
LOCALW	wUserHandle
LOCALV	sVersionInfo,OSVERSIONINFOW
OPENPROC
	PUSHR	esi,edi

	; Assume for a second Let's get USER
        mov     ax,_DATA
        shl     eax,10H
        mov     ax,OFFSET szUserModuleName
        push    eax
        call    GetModuleHandle
        mov	wUserHandle,ax

	; Now check the version of the OS
	mov	sVersionInfo.dwOSVersionInfoSize,SIZE OSVERSIONINFOA
	mov	ax,ss
	shl	eax,10H
	lea	ax,sVersionInfo
	push	eax
	call	GetVersionEx
	or	ax,ax
	je	DoneIt

	; Dispatch to correct OS handler
	mov	eax,sVersionInfo.dwMajorVersion
	mov	ecx,sVersionInfo.dwMinorVersion
	mov	edx,sVersionInfo.dwBuildNumber

	; Make sure it is Windows 9X
	cmp	eax,4
	jne	DoneIt

	; We don't care about Windows 95
	cmp	ecx,0			; Windows 95
	je	DoneIt

	; Check for the three versions of 9X we care about
	cmp	ecx,90			; Windows ME
	jne	NotWinME
	cmp	edx,45A0BB8H
	je	IsOK
	jmp	DoneIt

NotWinME:
	; Windows 98 and Windows 98SE have the same Major
	; and Minor version numbers and build number.
	cmp	ecx,10			; Windows 98,98SE
	jne	DoneIt
	cmp	edx,040A08AEH		; Windows 98,98SE
	jne	DoneIt

IsOK:
	; The technique to get the address of the flag is the same
	; in Win98, Win98SE and WinME so we just use one code path.
	push	wUserHandle
	pushd	602
	call	GetProcAddress
	mov	cx,dx
	or	cx,ax
	je	DoneIt

	; Lets' make sure we won't fault if we read it
	PUSHR	ax,dx
	push	dx
	push	ax
	pushw	30H
	call	IsBadReadPtr
	POPR	di,si
	or	ax,ax
	jne	DoneIt

	; Now we can load the ptr to the code.
	; Let's do a few checks to see if everything is cool.
	mov	fs,si
	cmp	byte ptr fs:[di + 5],0B8H
	jne	DoneIt
	cmp	word ptr fs:[di + 25H],6F6H
	jne	DoneIt

	; There should be a ptr to USER's data selector at offset 6
	mov	si,fs:[di + 6]
	mov	di,fs:[di + 27H]

	; Suppsedly si is the data selector we want. The bit that
	; says "I am in fullscreen DOS" is at si:76A bit 3. Let's
	; make sure that we can read and write that byte.
	push	si
	push	di
	pushw	1
	call	IsBadWritePtr
	or	ax,ax
	jne	DoneIt

	; Everything looks OK. Let's save the address
	mov     es,cs:[wNVTEXT16_Alias_In_NVTEXT16]
	shl	esi,10H
	mov	si,di
	mov	es:[lpUserFullscreenDOSBoxFlag],esi

DoneIt:
	POPR	esi,edi
CLOSEPROC

CLOSESEG _NVTEXT16

END

