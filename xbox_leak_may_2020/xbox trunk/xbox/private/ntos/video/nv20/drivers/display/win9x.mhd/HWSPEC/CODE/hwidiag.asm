;==============================================================================
;
; Copyright (C) 1998, Nvidia Coporation
;
; File:         hwidiag.asm
;
; Purpose:      This file implements diagnostics that Intel requires
;               for purchase of our boards. The functions in here
;               are all reached via a driver Escape mechanism.
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
include nvcm.inc

INTEL_DIAG_FRAME_AND_LOCALS	MACRO
PARMD	pHWLogdevCXOffset
PARMD   lpIn
PARMD   lpOut
ENDM

IDIAG_OPEN_INTERFACE            TEXTEQU <0>
IDIAG_CLOSE_INTERFACE           TEXTEQU <1>
IDIAG_GET_ERROR_CODE            TEXTEQU <2>
IDIAG_GET_CRC                   TEXTEQU <3>
IDIAG_GET_DRIVER_INFO           TEXTEQU <4>
IDIAG_GET_MEMORY_INFO           TEXTEQU <5>
IDIAG_GET_CLOCK_RATES           TEXTEQU <6>
IDIAG_GET_PCI_CONFIG            TEXTEQU <7>
IDIAG_GET_BUS_CONFIG            TEXTEQU <8>

IDIAG_ERROR_NONE                TEXTEQU <0>
IDIAG_ERROR_GENERIC             TEXTEQU <1>
IDIAG_ERROR_INVALID_PARAM       TEXTEQU <2>
IDIAG_ERROR_NOT_SUPPORTED       TEXTEQU <3>

; This is bad to redefine these here, but I am doing it for now!
; JKTODO ...
NV_PRAMDAC_TEST_CONTROL		TEXTEQU	<680608H>
NV_PRAMDAC_CHECKSUM		TEXTEQU	<68060CH>


OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT16, USE16, CODE

PUBLIC dwBuildYear, dwBuildMonth, dwBuildDay
TWO_DIGIT_YEAR      	TEXTEQU @SubStr( %@Date, 7, 2 )
CURRENT_YEAR    	TEXTEQU <2000 + TWO_DIGIT_YEAR>
dwBuildYear     	DD      CURRENT_YEAR
dwBuildMonth		DD	@SubStr( %@Date, 1, 2 )
dwBuildDay		DD	@SubStr( %@Date, 4, 2 )
szNVCompanyString       DB      NV_COMPANY_NAME_STRING_SHORT,0
szDriverVersion         DB      NV_VERSION_STRING,0

;==============================================================================
;
; Function:	NV_IntelDiagnostics
;
; Purpose:      This function is a set of Intel diagnostics.
;               Intel requires this Escape and diagnostics in
;               order to purchase our boards.
;
; Arguments:
;		ds			CX Sel
;		pHWLogdevCXOffset	HWLOGDEVCX ptr
;		lpIn			lpIn ptr of Escape function
;		lpOut			lpOut ptr of Escape function
; Returns:
;		ecx	0 	then pretend the escape was not implemented
;			non-0	then it was handled and eax has the
;				return code.
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC NV_IntelDiagnostics, PASCAL, FRAME, FAR16
INTEL_DIAG_FRAME_AND_LOCALS
OPENPROC
        PUSHR   esi,edi
        mov	ebx,pHWLogdevCXOffset

	; Verify the lpIn and lpOut parameters
	sub	eax,eax
	cmp	eax,lpIn
	je	FailIt
	cmp	eax,lpOut
	je	FailIt

	; Proceed with diagnostics subfunction
	lfs	si,lpIn
	les	di,lpOut
	mov	eax,fs:[si]

	push	OFFSET RetAddr
	cmp	eax,IDIAG_OPEN_INTERFACE
	je	NV_OpenIntelDiagInterface
	add	sp,@WordSize

	; If the interface is not open and this was not an open
	; request, the Intel spec says to pretend that the escape
	; is not implemented.
	cmp	ds:[ebx].HWLOGDEVCX.wDiagInterfaceOpen,0
	je	NotUs

	push	OFFSET RetAddr
	cmp	eax,IDIAG_CLOSE_INTERFACE
	je	NV_CloseIntelDiagInterface
	cmp	eax,IDIAG_GET_ERROR_CODE
	je	NV_IntelDiagGetErrorCode
	cmp	eax,IDIAG_GET_CRC
	je	NV_IntelDiagGetCRC
	cmp	eax,IDIAG_GET_DRIVER_INFO
	je	NV_IntelDiagDriverInfo
	cmp	eax,IDIAG_GET_MEMORY_INFO
	je      NV_IntelDiagMemoryInfo
	cmp	eax,IDIAG_GET_CLOCK_RATES
	je	NV_IntelDiagClockRates
	cmp	eax,IDIAG_GET_PCI_CONFIG
	je	NV_IntelDiagPciConfig
	cmp	eax,IDIAG_GET_BUS_CONFIG
	je	NV_IntelDiagBusConfig
	add	sp,@WordSize

NotSupported:
	mov	ds:[ebx].HWLOGDEVCX.wLastDiagError,IDIAG_ERROR_NOT_SUPPORTED
	mov	ecx,1
	mov	ax,-1
	jmp	DoneIt

NotUs:
	sub	ecx,ecx
	jmp	DoneIt

FailIt:
	; Comment to keep assembler from weird error
	mov	ds:[ebx].HWLOGDEVCX.wLastDiagError,IDIAG_ERROR_INVALID_PARAM
	mov	ecx,1
	sub	ax,ax
	jmp	DoneIt

RetAddr:
	mov	ecx,1

DoneIt:
	POPR	esi,edi
CLOSEPROC



;==============================================================================
;
; Function:     NV_OpenIntelDiagInterface
;
; Purpose:      This function is called if
;		lpIn->DIAGINPUT.dwFunction = IDIAG_OPEN_INTERFACE
;
; Arguments:
;		Same frame as NV_ControlIntelDiagnostics
;		ds:ebx	HWLOGDEVCX
;		fs:si	lpIn
;		es:di	lpOut
;
; Returns:
;		if	lpIn->DIAGINPUT.dwOpenKey == INTEL_DIAG_OPEN_KEY
;			lpOut->DIAGOUTPUT.dwOpenAck = INTEL_DIAG_OPEN_ACK
;		ax	Error Code to return
;
; Preserve:	Nothing
;==============================================================================
OPEN_INPUT	STRUCT
	dwFunction	DD	0
	dwOpenKey	DD	0
OPEN_INPUT	ENDS

OPEN_OUTPUT	STRUCT
	dwOpenAck	DD	0
OPEN_OUTPUT	ENDS

INTEL_DIAG_OPEN_KEY	TEXTEQU	<0B295F38DH>
INTEL_DIAG_OPEN_ACK	TEXTEQU	<070CA14E6h>

DECPROC NV_OpenIntelDiagInterface, PASCAL, ShareFrameNear16, NEAR
INTEL_DIAG_FRAME_AND_LOCALS
OPENPROC
	sub	cx,cx
	mov	ax,IDIAG_ERROR_INVALID_PARAM
	cmp	fs:[si].OPEN_INPUT.dwOpenKey,INTEL_DIAG_OPEN_KEY
	jne	SetErrorCode

	; return the correct value
        mov     es:[di].OPEN_OUTPUT.dwOpenAck,INTEL_DIAG_OPEN_ACK

	; Tell our logical device that the interface is open
	mov	ds:[ebx].HWLOGDEVCX.wDiagInterfaceOpen,1
	mov	ax,IDIAG_ERROR_NONE
	mov	ecx,1

SetErrorCode:
	mov	ds:[ebx].HWLOGDEVCX.wLastDiagError,ax
	mov	ax,cx
CLOSEPROC


;==============================================================================
;
; Function:     NV_CloseIntelDiagInterface
;
; Purpose:      This function is called if
;		lpIn->DIAGINPUT.dwFunction = IDIAG_CLOSE_INTERFACE
;
; Arguments:
;		Same frame as NV_ControlIntelDiagnostics
;		ds:ebx	HWLOGDEVCX
;		fs:si	lpIn
;		es:di	lpOut
;
; Returns:
;		Close the interface
;		Set the last error code to None
;		ax	Error Code to return
;
; Preserve:	Nothing
;==============================================================================
DECPROC NV_CloseIntelDiagInterface, PASCAL, ShareFrameNear16, NEAR
INTEL_DIAG_FRAME_AND_LOCALS
OPENPROC
	; Tell our logical device that the interface is closed
	mov	ds:[ebx].HWLOGDEVCX.wDiagInterfaceOpen,0
	mov	ds:[ebx].HWLOGDEVCX.wLastDiagError,IDIAG_ERROR_NONE
	mov	ax,1
CLOSEPROC


;==============================================================================
;
; Function:	NV_IntelDiagGetErrorCode
;
; Purpose:      This function is called if
;		lpIn->DIAGINPUT.dwFunction = IDIAG_GET_ERROR_CODE
;
; Arguments:
;		Same frame as NV_ControlIntelDiagnostics
;		ds:ebx	HWLOGDEVCX
;		fs:si	lpIn
;		es:di	lpOut
;
; Returns:
;		The output structure is filled in
;		ax	Error Code to return
;
; Preserve:	Nothing
;==============================================================================
GET_ERROR_CODE_OUTPUT	STRUCT
	dwError		DD	0
GET_ERROR_CODE_OUTPUT	ENDS

DECPROC NV_IntelDiagGetErrorCode, PASCAL, ShareFrameNear16, NEAR
INTEL_DIAG_FRAME_AND_LOCALS
OPENPROC
	; Tell our logical device that the interface is closed
	movzx	eax,ds:[ebx].HWLOGDEVCX.wLastDiagError
	mov	es:[di].GET_ERROR_CODE_OUTPUT.dwError,eax
	mov	ds:[ebx].HWLOGDEVCX.wLastDiagError,IDIAG_ERROR_NONE
	mov	ax,1
CLOSEPROC


;==============================================================================
;
; Function:	NV_IntelDiagGetCRC
;
; Purpose:      This function is called if
;		lpIn->DIAGINPUT.dwFunction = IDIAG_GET_CRC
;
; Arguments:
;		Same frame as NV_ControlIntelDiagnostics
;		ds:ebx	HWLOGDEVCX
;		fs:si	lpIn
;		es:di	lpOut
;
; Returns:
;		The output structure is filled in
;		ax	Error Code to return
;
; Preserve:	Nothing
;==============================================================================
GET_CRC_OUTPUT	STRUCT
	dwCRC0		DD	0	; Red   Channel (if applicable)
	dwCRC1		DD	0       ; Green Channel (if applicable)
	dwCRC2		DD	0       ; Blue  Channel (if applicable)
	dwCRC3		DD	0
GET_CRC_OUTPUT	ENDS

DECPROC NV_IntelDiagGetCRC, PASCAL, ShareFrameNear16, NEAR
INTEL_DIAG_FRAME_AND_LOCALS
OPENPROC
	; Comment to keep assembler from choking... Don't ask me why.
	; We already know that there are no errors in this sequence
	mov	ds:[ebx].HWLOGDEVCX.wLastDiagError,IDIAG_ERROR_NONE
        mov     esi,ds:[ebx].HWLOGDEVCX.pHWBoardCX
        mov     esi,ds:[esi].HWBOARDCX.pChipRegs

	; Read Red Channel
	mov	ecx,2
	call	GetCRC
	mov	es:[di].GET_CRC_OUTPUT.dwCRC0,eax

	; Read Green Channel
	mov	ecx,1
	call	GetCRC
	mov	es:[di].GET_CRC_OUTPUT.dwCRC1,eax

	; Read Blue Channel
	sub	ecx,ecx
	call	GetCRC
	mov	es:[di].GET_CRC_OUTPUT.dwCRC2,eax

	; I don't know what this one is for, but return 0 in it.
	sub	eax,eax
	mov	es:[di].GET_CRC_OUTPUT.dwCRC3,eax
	mov	ax,1
	jmp	DoneIt

GetCRC:
        pushf
        cli

Loopy:	test	dword ptr ds:[esi + 6013DAH],8
	jnz	Loopy
@@:	test	dword ptr ds:[esi + 6013DAH],8
	jz	@B

        mov	eax,11H
        or      ds:[esi + NV_PRAMDAC_TEST_CONTROL],eax
        mov	eax,ecx
        shl     eax,8
        or      eax,10h
        mov     ds:[esi + NV_PRAMDAC_TEST_CONTROL],eax
        sub	eax,eax
        mov     ds:[esi + NV_PRAMDAC_CHECKSUM],eax
        mov     eax,ds:[esi + NV_PRAMDAC_CHECKSUM]

        ; For TNT2 checksum bug.  If the reset bit is on.  Reset again
        test    eax,1 SHL 24
        jnz     Loopy

        ; Wait one frame
@@:	test	dword ptr ds:[esi + 6013DAH],8
	jnz	@B
@@:	test	dword ptr ds:[esi + 6013DAH],8
	jz	@B

	; read the checksum
        mov     eax,ds:[esi + NV_PRAMDAC_CHECKSUM]
        and     eax,0FFFFFFH

	; The popf should restore the interrupt state.
        popf
	ret
DoneIt:
CLOSEPROC


;==============================================================================
;
; Function:	NV_IntelDiagDriverInfo
;
; Purpose:      This function is called if
;		lpIn->DIAGINPUT.dwFunction = IDIAG_GET_DRIVER_INFO
;
; Arguments:
;		Same frame as NV_ControlIntelDiagnostics
;		ds:ebx	HWLOGDEVCX
;		fs:si	lpIn
;		es:di	lpOut
;
; Returns:
;		The output structure is filled in
;		ax	Error Code to return
;
; Preserve:	Nothing
;==============================================================================
GET_DRIVER_INFO_OUTPUT	STRUCT
	dwInterfaceRev	DD	0	; major in hi word, minor in lo word
	dwDay		DD	0	; e.g. 23
	dwMonth		DD	0       ; e.g. 2
	dwYear		DD	0       ; e.g. 1999
	szManufacturer	DB	80 DUP(0)
	szDriverVer	DB	80 DUP(0)
	szBiosVer 	DB	80 DUP(0)
GET_DRIVER_INFO_OUTPUT	ENDS

DECPROC NV_IntelDiagDriverInfo, PASCAL, ShareFrameNear16, NEAR
INTEL_DIAG_FRAME_AND_LOCALS
OPENPROC
	; We already know that there are no errors in this sequence
	mov	ds:[ebx].HWLOGDEVCX.wLastDiagError,IDIAG_ERROR_NONE
	mov	ecx,ebx

	mov	es:[di].GET_DRIVER_INFO_OUTPUT.dwInterfaceRev,00004H
	mov	eax,dwBuildDay
	mov	es:[di].GET_DRIVER_INFO_OUTPUT.dwDay,eax
	mov	eax,dwBuildMonth
	mov	es:[di].GET_DRIVER_INFO_OUTPUT.dwMonth,eax
	mov	eax,dwBuildYear
	mov	es:[di].GET_DRIVER_INFO_OUTPUT.dwYear,eax

	lea	si,[di].GET_DRIVER_INFO_OUTPUT.szManufacturer
	lea	bx,szNVCompanyString
@@:	mov	al,cs:[bx]
	inc	bx
	mov	es:[si],al
	inc	si
	or	al,al
	jne	@B

	lea	si,[di].GET_DRIVER_INFO_OUTPUT.szDriverVer
	lea	bx,szDriverVersion
@@:	mov	al,cs:[bx]
	inc	bx
	mov	es:[si],al
	inc	si
	or	al,al
	jne	@B

        mov     esi,ds:[ecx].HWLOGDEVCX.pHWBoardCX
        pushd   NV_CFG_BIOS_REVISION
        push    ds:[esi].HWBOARDCX.dwDeviceNum
        mov	ax,SEG lpfnNvConfigGet
        mov	gs,ax
        call    FFP16 PTR gs:[lpfnNvConfigGet]
        shl     edx,16
        mov     dx,ax
        mov     eax,edx
        les	di,lpOut

	mov	cx,8
Loopy:
	shld	edx,eax,4
	shl	eax,4
	and	edx,0FH
	cmp	edx,10
	sbb	ebx,ebx
	not	ebx
	and	ebx,7
	lea	edx,[edx + ebx + 30H]
	mov	es:[di].GET_DRIVER_INFO_OUTPUT.szBiosVer,dl
	inc	di

	cmp	cx,7
	je	DoDot
	cmp	cx,5
	jne	SkipDot
DoDot:
	mov	es:[di].GET_DRIVER_INFO_OUTPUT.szBiosVer,'.'
	inc	di

SkipDot:
	dec	cx
	jne	Loopy
	mov	ax,1
CLOSEPROC


;==============================================================================
;
; Function:	NV_IntelDiagMemoryInfo
;
; Purpose:      This function is called if
;		lpIn->DIAGINPUT.dwFunction = IDIAG_GET_MEMORY_INFO
;
; Arguments:
;		Same frame as NV_ControlIntelDiagnostics
;		ds:ebx	HWLOGDEVCX
;		fs:si	lpIn
;		es:di	lpOut
;
; Returns:
;		The output structure is filled in
;		ax	Error Code to return
;
; Preserve:	Nothing
;==============================================================================
GET_MEMORY_INFO_OUTPUT	STRUCT
	dwKSize		DD	0 ; size in Kilobytes
	dwType		DD	0 ; DRAM (EDO/FPM) = 1, SDRAM = 2, SGRAM = 3
GET_MEMORY_INFO_OUTPUT	ENDS

DECPROC NV_IntelDiagMemoryInfo, PASCAL, ShareFrameNear16, NEAR
INTEL_DIAG_FRAME_AND_LOCALS
OPENPROC
	; We already know that there are no errors in this sequence
	mov	ds:[ebx].HWLOGDEVCX.wLastDiagError,IDIAG_ERROR_NONE
        mov     esi,ds:[ebx].HWLOGDEVCX.pHWBoardCX

        mov     eax,ds:[esi].HWBOARDCX.dwVideoMemorySize
        shr	eax,10
	mov	es:[di].GET_MEMORY_INFO_OUTPUT.dwKSize,eax

        pushd   NV_CFG_RAM_TYPE
        push    ds:[esi].HWBOARDCX.dwDeviceNum
        mov     ax,SEG lpfnNvConfigGet
        mov     gs,ax
        call    FFP16 PTR gs:[lpfnNvConfigGet]
        les	di,lpOut
	mov	es:[di].GET_MEMORY_INFO_OUTPUT.dwType,eax

	mov	ax,1
CLOSEPROC


;==============================================================================
;
; Function:	NV_IntelDiagClockRates
;
; Purpose:      This function is called if
;		lpIn->DIAGINPUT.dwFunction = IDIAG_GET_CLOCK_RATES
;
; Arguments:
;		Same frame as NV_ControlIntelDiagnostics
;		ds:ebx	HWLOGDEVCX
;		fs:si	lpIn
;		es:di	lpOut
;
; Returns:
;		The output structure is filled in
;		ax	Error Code to return
;
; Preserve:	Nothing
;==============================================================================
GET_CLOCK_RATES_OUTPUT	STRUCT
	dwEngClockKHz	DD	0
	dwMemClockKHz	DD	0
GET_CLOCK_RATES_OUTPUT	ENDS

DECPROC NV_IntelDiagClockRates, PASCAL, ShareFrameNear16, NEAR
INTEL_DIAG_FRAME_AND_LOCALS
OPENPROC
	; We already know that there are no errors in this sequence
	mov	ds:[ebx].HWLOGDEVCX.wLastDiagError,IDIAG_ERROR_NONE
        mov     esi,ds:[ebx].HWLOGDEVCX.pHWBoardCX

        pushd   NV_CFG_DAC_GRAPHICS_CLOCK
        push    ds:[esi].HWBOARDCX.dwDeviceNum
        mov     ax,SEG lpfnNvConfigGet
        mov     gs,ax
        call    FFP16 PTR gs:[lpfnNvConfigGet]
        shl     edx,16
        mov     dx,ax
        mov     eax,edx
        mov     ecx,1000
        xor     edx,edx
        div     ecx
        les	di,lpOut
        mov     es:[di].GET_CLOCK_RATES_OUTPUT.dwEngClockKHz,eax

        pushd   NV_CFG_DAC_MEMORY_CLOCK
        push    ds:[esi].HWBOARDCX.dwDeviceNum
        mov     ax,SEG lpfnNvConfigGet
        mov     gs,ax
        call    FFP16 PTR gs:[lpfnNvConfigGet]
        shl     edx,16
        mov     dx,ax
        mov     eax,edx
        mov     ecx,1000
        xor     edx,edx
        div     ecx
        les	di,lpOut
        mov     es:[di].GET_CLOCK_RATES_OUTPUT.dwMemClockKHz,eax

	mov	ax,1
CLOSEPROC


;==============================================================================
;
; Function:	NV_IntelDiagPciConfig
;
; Purpose:      This function is called if
;		lpIn->DIAGINPUT.dwFunction = IDIAG_GET_PCI_CONFIG
;
; Arguments:
;		Same frame as NV_ControlIntelDiagnostics
;		ds:ebx	HWLOGDEVCX
;		fs:si	lpIn
;		es:di	lpOut
;
; Returns:
;		The output structure is filled in
;		ax	Error Code to return
;
; Preserve:	Nothing
;==============================================================================
GET_PCI_CONFIG_OUTPUT	STRUCT
	dwPCI0	DD	0	; PCI DEVICE hi 16 bits: PCI Vendor lo 16
	dwPCI1	DD	0	; PCI Class high 24 bits: PCI Revision lo 8
	dwPCI2	DD	0	; PCI SubSys hi 16 bits: PCI subsys vendor lo 16
	dwPCI3	DD	0	; PCI framebuffer address
GET_PCI_CONFIG_OUTPUT	ENDS

DECPROC NV_IntelDiagPciConfig, PASCAL, ShareFrameNear16, NEAR
INTEL_DIAG_FRAME_AND_LOCALS
OPENPROC
	; We already know that there are no errors in this sequence
	mov	ds:[ebx].HWLOGDEVCX.wLastDiagError,IDIAG_ERROR_NONE
        mov     esi,ds:[ebx].HWLOGDEVCX.pHWBoardCX
        mov     ecx,ds:[esi].HWBOARDCX.pChipRegs

        ; return PCI Device ID (upper 16), PCI Vendor ID (lower 16)
        mov     eax,ds:[ecx + 1800H]
        mov     es:[di].GET_PCI_CONFIG_OUTPUT.dwPCI0,eax

        ; return PCI Class Code high high 24 bits, Revision ID low 8 bits
        mov     eax,ds:[ecx + 1808H]
        mov     es:[di].GET_PCI_CONFIG_OUTPUT.dwPCI1,eax

        ; return PCI Subsystem ID (upper 16), PCI Subsystem Vendor ID (lower 16)
        mov     eax,ds:[ecx + 180CH]
        mov     es:[di].GET_PCI_CONFIG_OUTPUT.dwPCI2,eax

        ; return PCI Frame buffer base address
        mov     eax,ds:[ecx + 1814H]
        and     eax,0FF000000H
        mov     es:[di].GET_PCI_CONFIG_OUTPUT.dwPCI3,eax
        mov	ax,1
CLOSEPROC


;==============================================================================
;
; Function:	NV_IntelDiagBusConfig
;
; Purpose:      This function is called if
;		lpIn->DIAGINPUT.dwFunction = IDIAG_GET_BUS_CONFIG
;
; Arguments:
;		Same frame as NV_ControlIntelDiagnostics
;		ds:ebx	HWLOGDEVCX
;		fs:si	lpIn
;		es:di	lpOut
;
; Returns:
;		The output structure is filled in
;		ax	Error Code to return
;
; Preserve:	Nothing
;==============================================================================
GET_BUS_CONFIG_OUTPUT	STRUCT
	dwBusMode	DD	0 ; PCI 33MHZ = 1, PCI 66MHZ = 2, AGP = 3
	dwAGPRate	DD	0 ; NA = 0, 1X = 1, 2X = 2, 4X = 3
	dwAGPMode	DD	0 ; NA = 0, Pipelined = 1, Sideband = 2
	dwFastWrite	DD	0 ; NA = 0, Disabled = 1, Enabled = 2
GET_BUS_CONFIG_OUTPUT	ENDS

DECPROC NV_IntelDiagBusConfig, PASCAL, ShareFrameNear16, NEAR
INTEL_DIAG_FRAME_AND_LOCALS
OPENPROC
	; We already know that there are no errors in this sequence
	mov	ds:[ebx].HWLOGDEVCX.wLastDiagError,IDIAG_ERROR_NONE
        mov     esi,ds:[ebx].HWLOGDEVCX.pHWBoardCX
        mov     ecx,ds:[esi].HWBOARDCX.pChipRegs

        mov     eax,ds:[esi].HWBOARDCX.dwBusType
	cmp	eax,NV_BUS_TYPE_AGP
	jne	BusPci
BusAgp:
        mov     es:[di].GET_BUS_CONFIG_OUTPUT.dwBusMode,3

	; Bus rate is in chip reg at 184CH
        mov     eax,ds:[ecx + 184CH]
        mov     ebx,eax
        and     ebx,7
        test    ebx,4
        jz      @f
        mov     ebx,3
@@:     mov     es:[di].GET_BUS_CONFIG_OUTPUT.dwAGPRate,ebx

        ; AGP side band
        test    eax,1 SHL 8
        setnz   bl
        inc     ebx
	mov     es:[di].GET_BUS_CONFIG_OUTPUT.dwAGPMode,ebx

        ; AGP fast write
        test    eax,1 SHL 3
        setnz   bl
        inc     ebx
	mov     es:[di].GET_BUS_CONFIG_OUTPUT.dwFastWrite,ebx
	jmp	DoneIt

BusPci:
        ; Bus type is PCI
        mov     es:[di].GET_BUS_CONFIG_OUTPUT.dwBusMode,1

	sub	eax,eax
     	mov     es:[di].GET_BUS_CONFIG_OUTPUT.dwAGPRate,eax
	mov     es:[di].GET_BUS_CONFIG_OUTPUT.dwAGPMode,eax
	mov     es:[di].GET_BUS_CONFIG_OUTPUT.dwFastWrite,eax

DoneIt:
	mov	ax,1
CLOSEPROC

CLOSESEG _NVTEXT16

END

