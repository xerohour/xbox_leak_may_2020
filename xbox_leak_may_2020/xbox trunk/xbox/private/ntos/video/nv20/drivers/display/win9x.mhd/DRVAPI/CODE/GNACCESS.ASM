;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnaccess.asm
;
; Purpose:      This file implements the BeginAccess and EndAccess
;               Functions.
;
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include sysdata.inc
include gndata.inc
include gnlogdev.inc
include	gnboard.inc
include gncrtc.inc
include gngdi.inc
include hwgdi.inc

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

;==============================================================================
;
; Function:     GENERIC_BeginAccess
;
; Purpose:      This function is the BeginAccess display driver callback
;               function used by other modules that need to syncronize
;               frame buffer access and access to the hardware.
;
; Arguments:    BeginAccess display driver stack frame
;
; Returns:      None
;
; Preserve:     All registers
;==============================================================================
DECPROC GENERIC_BeginAccess, PASCAL, FRAME, FAR16
PARMD   lpDevice
PARMW   wLeft
PARMW   wTop
PARMW   wRight
PARMW   wBottom
PARMW   wFlags
OPENPROC
        PUSHR   ds,esi
        mov	ds,cs:[wFlatDataSel]
        mov	esi,cs:[CSpGNLogicalDevice]
	bts	ds:[esi].DIBENGINE.deFlags,BUSY_BIT
	jc	BA_End

        test    wFlags,FB_ACCESS
        jz      @F
        cmp     ds:[esi].GNLOGDEVCX.dwHardwareBusy,0
	jne	@F
        mov	esi,ds:[esi].GNLOGDEVCX.pHWLogdevCX
        call    HW_BeginAccess
@@:
	test	wFlags,CURSOREXCLUDE
	jz	@F

	mov	eax,lpDevice
	cmp	eax,cs:[CSlp16LogDevice]
	jne	@F

	push	lpDevice
	push	wLeft
	push	wTop
	push	wRight
	push	wBottom
	pushw	CURSOREXCLUDE
	call	DIB_BeginAccess
@@:
	; Turn off BUSY bit
        mov	esi,cs:[CSpGNLogicalDevice]
	and	ds:[esi].DIBENGINE.deFlags,NOT BUSY
BA_End:
        POPR    ds,esi
CLOSEPROC


;==============================================================================
;
; Function:     GENERIC_EndAccess
;
; Purpose:      This function is the EndAccess display driver callback
;               function. It is called by other modules after they have
;               finished accessing the frame buffer or other hardware.
;
; Arguments:    EndAccess display driver stack frame
;
; Returns:      None
;
; Preserve:     All registers
;==============================================================================
DECPROC GENERIC_EndAccess, PASCAL, FRAME, FAR16
PARMD   lpDevice
PARMW   wFlags
OPENPROC
	test	wFlags,CURSOREXCLUDE
	jz	EA_End

        PUSHR   ds,esi
        mov	ds,cs:[wFlatDataSel]
        mov	esi,cs:[CSpGNLogicalDevice]
	bts	ds:[esi].DIBENGINE.deFlags,BUSY_BIT
	jc	EA_Popr

        test    wFlags,FB_ACCESS
        jz      @F
        cmp     ds:[esi].GNLOGDEVCX.dwHardwareBusy,0
	jne	@F
        mov	esi,ds:[esi].GNLOGDEVCX.pHWLogdevCX
        call    HW_BeginAccess
@@:
	mov	eax,lpDevice
	cmp	eax,cs:[CSlp16LogDevice]
	jne	@F

	push	lpDevice
	pushw	CURSOREXCLUDE
	call	DIB_EndAccess

@@:
	; Turn off BUSY bit
        mov	esi,cs:[CSpGNLogicalDevice]
	and	ds:[esi].DIBENGINE.deFlags,NOT BUSY

EA_Popr:
        POPR    ds,esi
EA_End:
CLOSEPROC

CLOSESEG _NVTEXT32

END

