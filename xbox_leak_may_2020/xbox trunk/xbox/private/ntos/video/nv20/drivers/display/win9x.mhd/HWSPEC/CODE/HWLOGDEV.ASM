;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         hwlogdev.asm
;
; Purpose:      This file contains all the hardware specific
;               logical DEVICE specific init routines.
;==============================================================================
.586
incDevice = 1
include macros.dat
include gdidefs.inc
include dibeng.inc
include sysdata.inc
include nv32.inc
include hwlogdev.inc
include hwboard.inc
include hwcrtc.inc
include gngdi.inc
include hwgdi.inc
include files.inc
include hwmacs.inc
include nvcm.inc
include nvreg.inc
include modeext.inc
include dxshare.inc
include ..\..\..\..\..\sdk\nvidia\inc\nvrmarch.inc
include ..\..\..\..\..\sdk\nvidia\inc\rddapi.inc

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE

;==============================================================================
;
; Function:     HW_CreateLogdevContext
;
; Purpose:      This routine allocates memory for and initializes
;               a HWLOGDEVCX structure.
;
; Arguments:
;               ds                      Context Selector
;		pHWBoardCXOffset	HWBOARDCX ptr
;
; Returns:      eax     0       failure
;                       non-0   Success and eax is a ptr to the HWLOGDEVCX
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_CreateLogdevContext, PASCAL, FRAME, FAR16
PARMD	pHWBoardCXOffset
OPENPROC
        PUSHR   esi,edi
        sub     esi,esi

        pushd   SIZE HWLOGDEVCX
        call    HW_AllocFlatMemory
        or      eax,eax
        je      DoneIt
        mov     esi,eax

        ; We are initting
        mov     ds:[esi].HWLOGDEVCX.dwInitting,1

        ; We don't know this until an attach
        mov	eax,pHWBoardCXOffset
        mov     ds:[esi].HWLOGDEVCX.pHWBoardCX,eax

        ; DirectX is not active right now
        mov     ds:[esi].HWLOGDEVCX.dwDirectXActive,0

        ; Allocate memory for a mastr mode list to give to
        ; DirectX early on.
        pushd   2048 * (SIZE MODEENTRYANDPITCH)
        call    HW_AllocFlatMemory
        or      eax,eax
        je      DoneIt
        mov     ds:[esi].HWLOGDEVCX.pMasterModeList,eax

        ; Create a 16:16 ptr to the MasterModeList
        mov     eax,1
        mov     ebx,ds:[esi].HWLOGDEVCX.pMasterModeList
        mov     ecx,2048 * (SIZE MODEENTRYANDPITCH)
        call    GN_AllocSelectorAndMapIt
        or      eax,eax
        je      DoneIt
        shl     eax,10H
        mov     ds:[esi].HWLOGDEVCX.lpMasterModeList,eax

        ; Alloc memory for the DirectX shared data structure
        pushd   SIZE DISPDRVDIRECTXCOMMON
        call    HW_AllocFlatMemory
        or      eax,eax
        je      DoneIt
        mov     ds:[esi].HWLOGDEVCX.pDirectXSharedData,eax

        ; Create a 16:16 ptr to the Shared Data
        mov     eax,1
        mov     ebx,ds:[esi].HWLOGDEVCX.pDirectXSharedData
        mov     ecx,SIZE DISPDRVDIRECTXCOMMON
        call    GN_AllocSelectorAndMapIt
        or      eax,eax
        je      DoneIt
        shl     eax,10H
        mov     ds:[esi].HWLOGDEVCX.lpDirectXSharedData,eax

        ; Fill in the DirectX Shared Data
        mov     edi,ds:[esi].HWLOGDEVCX.pDirectXSharedData
        mov     ds:[edi].DISPDRVDIRECTXCOMMON.dwDeviceHandle,NV_WIN_DEVICE
        mov     eax,ds:[esi].HWLOGDEVCX.pMasterModeList
        mov     ds:[edi].DISPDRVDIRECTXCOMMON.lpMasterModeList,eax
	mov	ax,cs
	and	eax,0FFFFH
	mov	ds:[edi].DISPDRVDIRECTXCOMMON.dwDrvCallbackSelector,eax
	mov	eax,OFFSET NV_DirectXServiceNoFrame
	mov	ds:[edi].DISPDRVDIRECTXCOMMON.dwDrvCallbackOffset,eax
	mov	ds:[edi].DISPDRVDIRECTXCOMMON.pDispDriverHWLogDevCX,esi

	; Ask the HWBOARD to fill in its DXShare data
        push	ds:[esi].HWLOGDEVCX.pHWBoardCX
        push	ds:[esi].HWLOGDEVCX.pDirectXSharedData
        call	NV_BoardSetDXSharedData

        mov     eax,esi
DoneIt:
        ; If eax is 0, then we failed, BUT if esi is non-0, then
        ; some of the HWLOGDEVCX. was filled in. We must handle
        ; this case and free any portion that got initted.
        or      eax,eax
        jne     ExitIt
        or      esi,esi
        je      ExitIt

        ; HW_DestroyLogdevContext can clean up partial inits
        push    esi
        call    HW_DestroyLogdevContext
        sub     eax,eax

ExitIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:     HW_DestroyLogdevContext
;
; Purpose:      This routine allocates memory for and initializes
;               an array of HWCRTCCX structure.
;
; Arguments:
;               ds                      The context selector.
;               pHWLogdevCXOffset       offset of HWLOGDEVCX to destroy
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_DestroyLogdevContext, PASCAL, FRAME, FAR16
PARMD   pHWLogdevCXOffset
OPENPROC
        PUSHR   esi,edi
        mov     esi,pHWLogdevCXOffset
        or      esi,esi
        je      DoneIt

        ; Destroy the 16:16 alias for the shared data
        cmp     ds:[esi].HWLOGDEVCX.lpDirectXSharedData,0
        je      @F

        movzx   eax,word ptr ds:[esi].HWLOGDEVCX.lpDirectXSharedData+2
        call    GN_UnmapAndFreeSelector
        mov     ds:[esi].HWLOGDEVCX.lpDirectXSharedData,0
@@:
        ; Destroy the shared data memory
        cmp     ds:[esi].HWLOGDEVCX.pDirectXSharedData,0
        je      @F

        push    ds:[esi].HWLOGDEVCX.pDirectXSharedData
        call    HW_FreeFlatMemory
        mov     ds:[esi].HWLOGDEVCX.pDirectXSharedData,0
@@:

        ; Destroy the 16:16 alias for the shared data
        cmp     ds:[esi].HWLOGDEVCX.lpMasterModeList,0
        je      @F

        movzx   eax,word ptr ds:[esi].HWLOGDEVCX.lpMasterModeList+2
        call    GN_UnmapAndFreeSelector
        mov     ds:[esi].HWLOGDEVCX.lpMasterModeList,0

@@:
        ; Destroy the modelist memory
        cmp     ds:[esi].HWLOGDEVCX.pMasterModeList,0
        je      @F

        push    ds:[esi].HWLOGDEVCX.pMasterModeList
        call    HW_FreeFlatMemory
        mov     ds:[esi].HWLOGDEVCX.pMasterModeList,0
@@:
        push    esi
        call    HW_FreeFlatMemory

DoneIt:
        POPR    esi,edi
CLOSEPROC


;==============================================================================
;
; Function:	HW_LogdevAttachCrtc
;
; Purpose:      This function is called to tell the HWLOGDEVCX a
;		new HWCRTCCX.
;
; Arguments:
;               ds                      Context Selector
;               pHWLogdevCXOffset       HWLOGDEVCX ptr
;		dwLogicalCRTCIndex	The logical index for this
;					attachment (0, 1, 2, etc..
;		pHWCrtcCXOffset		HWCRTCCX ptr
;
; Returns:      eax			0 Fail
;					non-0 success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_LogdevAttachCrtc, PASCAL, FRAME, FAR16
PARMD   pHWLogdevCXOffset
PARMD	dwLogicalCRTCIndex
PARMD	pHWCrtcCXOffset
OPENPROC
	; First, put the HWCRTCCX into the HWLOGDEVCX
	mov	ebx,pHWLogdevCXOffset
	mov	eax,dwLogicalCRTCIndex
	mov	ecx,pHWCrtcCXOffset
	mov	ds:[ebx].HWLOGDEVCX.pHWCrtcCX[eax * 4],ecx

	; This HWCRTCCX has a parent HWLOGDEVCX
	push	pHWCrtcCXOffset
	push    pHWLogdevCXOffset
	call	NV_CrtcSetLogdev

	mov	eax,1
CLOSEPROC


;==============================================================================
;
; Function:	HW_LogdevDetachCrtc
;
; Purpose:      This function is called to tell the HWLOGDEVCX to
;		remove a HWCRTCCX from its list of active contexts.
;
; Arguments:
;               ds                      Context Selector
;               pHWLogdevCXOffset       HWLOGDEVCX ptr
;		dwLogicalCRTCIndex	The logical index for this
;					attachment (0, 1, 2, etc..
;		pHWCrtcCXOffset		HWCRTCCX ptr
;
; Returns:	None
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_LogdevDetachCrtc, PASCAL, FRAME, FAR16
PARMD   pHWLogdevCXOffset
PARMD	dwLogicalCRTCIndex
PARMD	pHWCrtcCXOffset
LOCALD	dwPitchInBytes
OPENPROC
	mov	ebx,pHWLogdevCXOffset
	mov	eax,dwLogicalCRTCIndex
	sub	ecx,ecx
	mov	ds:[ebx].HWLOGDEVCX.pHWCrtcCX[eax * 4],ecx

	; This HWCRTCCX no longer has a parent HWLOGDEVCX
	sub	eax,eax
	push	pHWCrtcCXOffset
	push    eax
	call	NV_CrtcSetLogdev
CLOSEPROC


;==============================================================================
;
; Function:     HW_Get16BppPixelFormat
;
; Purpose:      This function should return information about whether
;               the 16bpp mode is 1-5-5-5 or 5-6-5. Note that this is
;               not a per CRTC choice. It is per Logical Device choice.
;
; Arguments:
;               ds                      Context Selector
;               pHWLogdevCXOffset       HWLOGDEVCX ptr
;
; Returns:
;               eax     FIVE6FIVE if supporting 5-6-5
;                       0  if supporting 1-5-5-5
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_Get16BppPixelFormat, PASCAL, FRAME, FAR16
PARMD   pHWLogdevCXOffset
OPENPROC
        mov     eax,FIVE6FIVE
CLOSEPROC


;==============================================================================
;
; Function:     HW_GetTextFormat
;
; Purpose:      This function should return information about whether
;               the controller handles bit packed or byte packed
;               text. Bit packed is much better if you can support it.
;
; Arguments:
;               ds                      Context Selector
;               pHWLogdevCXOffset       HWLOGDEVCX ptr
;
; Returns:
;               eax     C1_BIT_PACKED
;                       or
;                       C1_BYTE_PACKED
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC HW_GetTextFormat, PASCAL, FRAME, FAR16
PARMD   pHWLogdevCXOffset
OPENPROC
        mov     eax,C1_BIT_PACKED
CLOSEPROC

CLOSESEG _NVTEXT16

END

