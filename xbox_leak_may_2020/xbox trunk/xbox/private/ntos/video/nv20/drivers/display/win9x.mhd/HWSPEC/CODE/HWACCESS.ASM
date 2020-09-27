;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         hwaccess.asm
;
; Purpose:      This file implements the BeginAccess Functions and
;               functions for creating and deleting offscreen bitmaps.
;
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include sysdata.inc
include nv32.inc
include hwlogdev.inc
include hwboard.inc
include hwcrtc.inc
include hwmacs.inc
include gngdi.inc
include hwgdi.inc

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE

;==============================================================================
;
; Function:     HW_GetGraphicsTimeStamp
;
; Purpose:      This function returns should return a value that is
;               some measure of graphics activity time. The DMA buffer
;               ptr is good since it increases with increasing graphics
;               activity (except when it is reset to the start of the
;               buffer, but I'll ignore that for now.)
;
; Arguments:
;               ds                      CXSel
;               pHWLogdevCXOffset       HWLOGDEVCX ptr
;
; Returns:
;               eax     A graphics time stamp
;
; Preserve:     All registers except eax
;==============================================================================
DECPROC HW_GetGraphicsTimeStamp, PASCAL, FRAME, NEAR
PARMD   pHWLogdevCXOffset
OPENPROC
        mov     eax,pHWLogdevCXOffset
        mov     eax,ds:[eax].HWLOGDEVCX.pHWBoardCX
        mov     eax,ds:[eax].HWBOARDCX.pCurrentPushBufferOffset
CLOSEPROC

CLOSESEG _NVTEXT16

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

;==============================================================================
;
; Function:     HW_BeginAccess
;
; Purpose:      This function should wait until all hardware contexts
;               are idle before returning. It is probably being called
;               so that the CPU can access the frame buffer directly
;               so wait for idle here to preserve order of operations.
;
; Arguments:
;               ds 	CXSel
;               esi     HWLOGDEVCX ptr
;
; Returns:      None
;
; Preserve:     All registers
;==============================================================================
DECPROC HW_BeginAccess, PASCAL, NOFRAME, NEAR
OPENPROC
	PUSHR	eax,ecx

        mov     eax,ds:[esi].HWLOGDEVCX.pHWBoardCX
        mov	ecx,ds:[eax].HWBOARDCX.dwGdiDriverChannelID
        cmp	ecx,ds:[eax].HWBOARDCX.dwCurrentChannelID
        jne	SyncOtherChannels

	; If we are still idle from the last sync, then don't sync again
        mov     ecx,ds:[eax].HWBOARDCX.pCurrentPushBufferOffset
        cmp     ecx,ds:[eax].HWBOARDCX.dwGetOffsetAtLastBeginAccess
        jne     SyncThisChannel

Done:
	STOPPROC
	POPR	eax,ecx
	ret

SyncThisChannel:
        PUSHR   alld

	; set edi = HWBOARDCX and edi = PushBufferOffset
        mov	esi,eax
	mov	edi,ecx

        CHECKDMASPACE ds,esi,edi,10H
        SETDMASTATUS <ds:[esi].HWBOARDCX.sPushBufferNotifier.status>
        mov     eax,CHNRUN(ContextClipRect.Notify,1)
        mov     ecx,NV019_NOTIFY_WRITE_ONLY
        mov     edx,CHNRUN(ContextClipRect.NoOperation,1)
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ecx
        mov     ds:[edi + 08H],edx
        mov     ds:[edi + 0CH],esi
        add     edi,10H

        ; Cyrix reorders cached reads ahead of uncached writes. If we
        ; don't execute a serializing instruction, we might spin on the
        ; notifier preventing the command from getting out to the chip.
        xchg    eax,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        SETPUTPTR ds,esi,edi

        ; Now wait until the notify is complete
        WAITDMASTATUS <ds:[esi].HWBOARDCX.sPushBufferNotifier.status>

        ; Remember the last "synced" get offset. People that want to
        ; write the frame buffer directly can compare the
        ; dwCurrentPushBufferOffset with this value. If they are the
        ; same then, there is no need to issue another sync.
        mov     ds:[esi].HWBOARDCX.dwGetOffsetAtLastBeginAccess,edi
        POPR    alld
	jmp	Done

SyncOtherChannels:
        WAIT_UNTIL_OTHER_CONTEXTS_IDLE ds,eax,ecx
	jmp	Done
CLOSEPROC

;==============================================================================
DECPROC HW_BeginAccess_Far32, PASCAL, NOFRAME, FAR32
OPENPROC
	call	HW_BeginAccess
CLOSEPROC

;==============================================================================
;
; Function:     HW_EndAccess
;
; Purpose:      This is the hardware specific EndAccess routine.
;               There is probably nothing you need to do here,
;               but it is called after the CPU is done touching
;               the display so if you need to put the hardware
;               back in "bltter" mode, do so now.
;
; Arguments:
;               ds              Context Selector
;               ebx             HWBOARDCX ptr
;
; Returns:      None
;
; Preserve:     All registers
;==============================================================================
DECPROC HW_EndAccess, PASCAL, NOFRAME, NEAR
OPENPROC
CLOSEPROC

IF	(HW_WAIT NE 0)
;==============================================================================
;
; Function:     WaitUntilGdiContextIdle
;
; Purpose:      This function should wait until all hardware contexts
;               are idle before returning. It is probably being called
;               so that the CPU can access the frame buffer directly
;               so wait for idle here to preserve order of operations.
;
; Arguments:
;               pHWBoardCXOffset
;
; Returns:      None
;
; Preserve:     All registers
;==============================================================================
DECPROC WaitUntilGdiContextIdle, PASCAL, FRAME, NEAR
PARMD   pHWBoardCXOffset
OPENPROC
	PUSHR	ds,alld

        mov     ds,cs:[wFlatDataSel]
        mov     eax,pHWBoardCXOffset

	; If we are still idle from the last sync, then don't sync again
        mov     ecx,ds:[eax].HWBOARDCX.pCurrentPushBufferOffset
        cmp     ecx,ds:[eax].HWBOARDCX.dwGetOffsetAtLastBeginAccess
        je	Done

	; set edi = HWBOARDCX and edi = PushBufferOffset
        mov	esi,eax
	mov	edi,ecx

        CHECKDMASPACE ds,esi,edi,10H
        SETDMASTATUS <ds:[esi].HWBOARDCX.sPushBufferNotifier.status>
        mov     eax,CHNRUN(ContextClipRect.Notify,1)
        mov     ecx,NV019_NOTIFY_WRITE_ONLY
        mov     edx,CHNRUN(ContextClipRect.NoOperation,1)
        mov     ds:[edi + 00H],eax
        mov     ds:[edi + 04H],ecx
        mov     ds:[edi + 08H],edx
        mov     ds:[edi + 0CH],esi
        add     edi,10H

        ; Cyrix reorders cached reads ahead of uncached writes. If we
        ; don't execute a serializing instruction, we might spin on the
        ; notifier preventing the command from getting out to the chip.
        xchg    eax,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        SETPUTPTR ds,esi,edi

        ; Now wait until the notify is complete
        WAITDMASTATUS <ds:[esi].HWBOARDCX.sPushBufferNotifier.status>

IFDEF DEBUG;!!
	;;Trap for MMIO corruption
        mov     eax,ds:[esi].HWBOARDCX.pDmaChannel
	sub	eax,0F0000H
	;;Read RM objects data
	mov	ebx,ds:[eax+50H]
	or	ebx,ebx
	jz	@F
        mov     ecx,ds:[esi].HWBOARDCX.dwGetOffsetAtLastBeginAccess
	int	3
@@:
ENDIF
        ; Remember the last "synced" get offset. People that want to
        ; write the frame buffer directly can compare the
        ; dwCurrentPushBufferOffset with this value. If they are the
        ; same then, there is no need to issue another sync.
        mov     ds:[esi].HWBOARDCX.dwGetOffsetAtLastBeginAccess,edi

Done:
	POPR	ds,alld
CLOSEPROC
ENDIF


;==============================================================================
;
; Function:     HW_CreateOffscreenBitmap
;
; Purpose:      This function is called by the generic code when it wants
;               to create an offscreen bitmap. You need to do two things
;               in here:
;               1) If the hardware cannot accept any bitmap from 1x1 up
;                  to 32K pixels by 32K pixels then check the bitmap size
;                  and succeed or fail appropriately.
;               2) If you accept the creation, fill in the
;                  ds:[esi].DIBENGINE.deDeltaScan field with the pitch
;                  you wish for the bitmap.
;
; Arguments:
;               ds              Context Selector
;               ebx             HWBOARDCX ptr
;               es:edi          ptr to DEVBMP.
;                               The fields below are valid:
;               deWidth
;               deHeight
;               deWidthBytes
;               dePlanes
;               deBitsPixel
;               delpPDevice
;
; Returns:      eax     0       Do not place this bitmap in offscreen
;                       non-0   OK to put the bitmap in offscreen
;                               In this case you should also fill in
;                               ds:[esi].DIBENGINE.deDeltaScan with
;                               the desired bitmap pitch. The pitch
;                               must be a multiple of 4, but your
;                               hardware may have other restrictions
;                               as well.
;
; Preserve:     ds,esi,edi,es
;               Do not destroy any other fields in the DEVBMP or PDEVICE
;==============================================================================
DECPROC HW_CreateOffscreenBitmap, PASCAL, NOFRAME, NEAR
OPENPROC
        movzx   eax,es:[edi].DIBENGINE.deWidthBytes
        mov     ecx,ds:[ebx].HWBOARDCX.dwMinContextSurface2dPitch
        dec     ecx
        add     eax,ecx
        not     ecx
        and     eax,ecx
        mov     es:[edi].DIBENGINE.deDeltaScan,eax
        dec     eax
        sub     eax,ds:[ebx].HWBOARDCX.dwMaxContextSurface2dPitch
        sbb     eax,eax
CLOSEPROC


;==============================================================================
;
; Function:     HW_DeleteOffscreenBitmap
;
; Purpose:      This routine is called when an offscreen bitmap is being
;               deleted. The hardware specific code can do anything it
;               needs to. Typically, you will need to check to see if
;               the current active bitmap has been set to the one being
;               deleted and set the active base and pitch in the chip
;               to, say, -1. Another bitmap may be allocated at the
;               same address with a different pitch and unless the
;               base and pitch get reset properly, things could get
;               drawn incorrectly.
;
; Arguments:
;               ds              Context Selector
;               ebx             HWBOARDCX ptr
;               eax		DIBENGINEHDR.deBitsOffset
;
; Returns:      None
;
; Preserve:     ds,esi,edi,es
;==============================================================================
DECPROC HW_DeleteOffscreenBitmap, PASCAL, NOFRAME, NEAR
OPENPROC
        cmp     eax,ds:[ebx].HWBOARDCX.dwContextSurface2dSrcOffset
        jne     @F
        mov     ds:[ebx].HWBOARDCX.dwContextSurface2dSrcOffset,0FFFF0000h
@@:
        cmp     eax,ds:[ebx].HWBOARDCX.dwContextSurface2dDstOffset
        jne     @F
        mov     ds:[ebx].HWBOARDCX.dwContextSurface2dDstOffset,0FFFF0000h
@@:
CLOSEPROC

CLOSESEG _NVTEXT32

END

