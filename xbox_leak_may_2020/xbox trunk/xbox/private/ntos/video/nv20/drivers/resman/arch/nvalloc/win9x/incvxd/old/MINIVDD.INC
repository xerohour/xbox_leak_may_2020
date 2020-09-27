;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1988-1993
;
;   Title:	minivdd.inc - VDD definitions for other VxD's and multiple VDD's
;
;   Version:	4.00
;
;   Date:
;
;   Author: FredE for the Mini-VDD interface.  Adapted from VDD.INC in
;	    the general include area.  Added services are ONLY for the
;	    use of the Mini-VDD and should therefore not be documented.
;
;------------------------------------------------------------------------------
;==============================================================================
;
;Some external definitions.  Only define these if we're assembling the
;device independent "main" portion of the Mini-VDD:
;
ifdef MAINVDD
externdef	MiniVDDDispatchTable:dword	;in VDDCTL.ASM
endif ;MAINVDD

;
;VDD protect mode services for other devices and VMM (Get_Version must be first).
;Only define these if we're assembling either the "main" VDD or one of the
;mini-VDD's.  Don't define this if we're assembling a display driver or other
;caller of the MiniVDD stuff:
;
ifdef MINIVDD
Begin_Service_Table VDD
VDD_Service	VDD_Get_Version 	; Get version number and ID string ptr
VDD_Service	VDD_PIF_State		; Pass video PIF bits to VDD
VDD_Service	VDD_Get_GrabRtn 	; Get routine addr for video grab
VDD_Service	VDD_Hide_Cursor 	; Hide cursor when display is windowed
VDD_Service	VDD_Set_VMType		; Set VM type(windowed, bckgrnd, excl)
VDD_Service	VDD_Get_ModTime 	; Return system time of last mod
VDD_Service	VDD_Set_HCurTrk 	; Track horiz cursor movement in window
VDD_Service	VDD_Msg_ClrScrn 	; Clear screen for sysmodal message
VDD_Service	VDD_Msg_ForColor	; Set Msg_TextOut forground color
VDD_Service	VDD_Msg_BakColor	; Set Msg_TextOut background color
VDD_Service	VDD_Msg_TextOut 	; Output a string
VDD_Service	VDD_Msg_SetCursPos	; Set cursor position
VDD_Service	VDD_Query_Access	; Is it OK to access video now?
VDD_Service	VDD_Check_Update_Soon	; User action may update screen
VDD_Service	VDD_Get_Mini_Dispatch_Table
					; Get addr of dispatch table to mini-VDD
VDD_Service	VDD_Register_Virtual_Port
					; Mini-VDD asks us to setup I/O trap
VDD_Service	VDD_Get_VM_Info 	; Returns CRTC owner and MemC owners
					; and other special VM handles
VDD_Service	VDD_Get_Special_VM_IDs	;returns planar & msg mode IDs
VDD_Service	VDD_Register_Extra_Screen_Selector
					;allows display driver to use second
					;screen selector for block moves
VDD_Service	VDD_Takeover_VGA_Port	;allows MiniVDD to takeover a port
					;in range 3C0H through 3DFH
VDD_Service	VDD_Get_DISPLAYINFO	;get DISPLAYINFO data structure
VDD_Service	VDD_Do_Physical_IO	;perform physical I/O for trapped port
VDD_Service	VDD_Set_Sleep_Flag_Addr ;when display driver can't be interrupted
End_Service_Table VDD
endif	;MINIVDD

IFDEF VDD40
VDD_VerNum	EQU	0400h	;version 4.00
ELSE
VDD_VerNum	=	030ah	;return version 3.10 if Makefile switch not set

IFDEF DEBUG
IFNDEF VGA
IFNDEF EGA
VDD_VerNum = VDD_VerNum + 0FFh
ENDIF ;EGA
ENDIF ;VGA
ENDIF ;DEBUG

ENDIF	;VDD40

;***************
; PIF_State service definitions
;
; These definitions cannot change without changing the PIF editor!!!
;
fVidTxtEmulate	EQU	0001h		; Do INT 10h TTY and cursor emulation
fVidNoTrpTxt	EQU	0002h		; Do not trap text mode apps
fVidNoTrpLRGrfx EQU	0004h		; Do not trap lo res graphics mode apps
fVidNoTrpHRGrfx EQU	0008h		; Do not trap hi res graphics mode apps
fVidTextMd	EQU	0010h		; Allocate text mode mem
fVidLowRsGrfxMd EQU	0020h		; Allocate lo res graphics mode mem
fVidHghRsGrfxMd EQU	0040h		; Allocate hi res graphics mode mem
fVidRetainAllo	EQU	0080h		; Never deallocate once allocated

;
;The following stuff was added for mini-VDD support:
;
;Functions that we can call in the hardware-dependent mini-VDD.  Note that
;these equates are used to create the dispatch table for calling functions
;in the mini-VDD:
;
REGISTER_DISPLAY_DRIVER 	equ	0
GET_VDD_BANK			equ	1
SET_VDD_BANK			equ	2
RESET_BANK			equ	3
PRE_HIRES_TO_VGA		equ	4
POST_HIRES_TO_VGA		equ	5
PRE_VGA_TO_HIRES		equ	6
POST_VGA_TO_HIRES		equ	7
SAVE_REGISTERS			equ	8
RESTORE_REGISTERS		equ	9
MODIFY_REGISTER_STATE		equ	10
ACCESS_VGA_MEMORY_MODE		equ	11
ACCESS_LINEAR_MEMORY_MODE	equ	12
ENABLE_TRAPS			equ	13
DISABLE_TRAPS			equ	14
MAKE_HARDWARE_NOT_BUSY		equ	15
VIRTUALIZE_CRTC_IN		equ	16
VIRTUALIZE_CRTC_OUT		equ	17
VIRTUALIZE_SEQUENCER_IN 	equ	18
VIRTUALIZE_SEQUENCER_OUT	equ	19
VIRTUALIZE_GCR_IN		equ	20
VIRTUALIZE_GCR_OUT		equ	21
SET_LATCH_BANK			equ	22
RESET_LATCH_BANK		equ	23
SAVE_LATCHES			equ	24
RESTORE_LATCHES 		equ	25
DISPLAY_DRIVER_DISABLING	equ	26
SELECT_PLANE			equ	27
PRE_CRTC_MODE_CHANGE		equ	28
POST_CRTC_MODE_CHANGE		equ	29
VIRTUALIZE_DAC_OUT		equ	30
VIRTUALIZE_DAC_IN		equ	31
GET_CURRENT_BANK_WRITE		equ	32
GET_CURRENT_BANK_READ		equ	33
SET_BANK			equ	34
CHECK_HIRES_MODE		equ	35
GET_TOTAL_VRAM_SIZE		equ	36
GET_BANK_SIZE			equ	37
SET_HIRES_MODE			equ	38
PRE_HIRES_SAVE_RESTORE		equ	39
POST_HIRES_SAVE_RESTORE 	equ	40
VESA_SUPPORT			equ	41
GET_CHIP_ID			equ	42
CHECK_SCREEN_SWITCH_OK		equ	43
VIRTUALIZE_BLTER_IO		equ	44
SAVE_MESSAGE_MODE_STATE 	equ	45
SAVE_FORCED_PLANAR_STATE	equ	46
VESA_CALL_POST_PROCESSING	equ	47
;
NBR_MINI_VDD_FUNCTIONS		equ	48	;REMEMBER TO RESET THIS!!!!!!

;
;Following are function codes that can be called via the VDD's
;API entry point. These are mainly for display driver --> VDD communication.
;Since Windows 3.0 and 3.1 VDD's may have used the sequential numbers
;(after the Grabber functions) for other VDD API services, we start our
;numbering at 80H so as to avoid ugly conflicts with old 3.1 stuff:
;
MINIVDD_SVC_BASE_OFFSET 		equ	80h
VDD_DRIVER_REGISTER			equ	0 or MINIVDD_SVC_BASE_OFFSET
VDD_DRIVER_UNREGISTER			equ	1 or MINIVDD_SVC_BASE_OFFSET
VDD_SAVE_DRIVER_STATE			equ	2 or MINIVDD_SVC_BASE_OFFSET
VDD_REGISTER_DISPLAY_DRIVER_INFO	equ	3 or MINIVDD_SVC_BASE_OFFSET
VDD_REGISTER_SSB_FLAGS			equ	4 or MINIVDD_SVC_BASE_OFFSET
VDD_GET_DISPLAY_CONFIG			equ	5 or MINIVDD_SVC_BASE_OFFSET
VDD_PRE_MODE_CHANGE			equ	6 or MINIVDD_SVC_BASE_OFFSET
VDD_POST_MODE_CHANGE			equ	7 or MINIVDD_SVC_BASE_OFFSET
VDD_SET_USER_FLAGS			equ	8 or MINIVDD_SVC_BASE_OFFSET
VDD_SET_BUSY_FLAG_ADDR			equ	9 or MINIVDD_SVC_BASE_OFFSET
;
;Port size equates:
;
BYTE_LENGTHED			equ	1
WORD_LENGTHED			equ	2

;
;Flag equates:
;
GOING_TO_WINDOWS_MODE		equ	1
GOING_TO_VGA_MODE		equ	2
DISPLAY_DRIVER_DISABLED 	equ	4
;
;
MiniVDDDispatch 	macro	FunctionCode, HandlerAddr
	mov	[edi+(FunctionCode*4)],OFFSET32 MiniVDD_&HandlerAddr
endm
;
;
MiniVDDCall		macro	FunctionCode, SaveFlags
local	MiniVDDCallExit, MiniVDDCallLeave
	push	edi			;;save this register for now
ifnb	<SaveFlags>
	pushfd				;;save the flags state
endif
	mov	edi,OFFSET32 MiniVDDDispatchTable
	cmp	dword ptr [edi+(FunctionCode*4)],0
	je	MiniVDDCallLeave	;;MiniVDD doesn't support this
ifnb	<SaveFlags>
	popfd				;;just clear the Stack from the flags
endif
	call	dword ptr [edi+(FunctionCode*4)]
ifnb	<SaveFlags>
	jmp	MiniVDDCallExit 	;;we already restored the flags
endif
;
MiniVDDCallLeave:
ifnb	<SaveFlags>
	popfd				;;
endif
;
MiniVDDCallExit:
	pop	edi			;;we're done handling this call
endm

DISPLAYINFO	struc
	diHdrSize		dw	size DisplayInfo
	diInfoFlags		dw	?
	diDevNodeHandle		dd	?
	diDriverName		db	16 dup(?)
	diXRes			dw	?
	diYRes			dw	?
	diDPI			dw	?
	diPlanes		db	?
	diBpp			db	?
	diRefreshRateMax	dw	?
	diRefreshRateMin	dw	?
	diLowHorz		dw	?
	diHighHorz		dw	?
	diLowVert		dw	?
	diHighVert		dw	?
	diMonitorDevNodeHandle	dd	?
	diHorzSyncPolarity	db	?
	diVertSyncPolarity	db	?
DISPLAYINFO	ends
DISPLAYINFO_SIZE	equ	diRefreshRateMax+2-diHdrSize
;
;Following are values and explanations for the diInfoFlags word in DISPLAYINFO:
;
;RETURNED_DATA_IS_STALE, if set, means that this call to VDD_GET_DISPLAY_CONFIG
;or VDD_GetDisplayInfo (which are the Ring 3 and Ring 0 methods by which a
;program would get the DISPLAYINFO structure returned to him) caused the VDD
;to return data that was read in a previous call to VDD_GET_DISPLAY_CONFIG
;insted of actually going out and reading "fresh" data from the Registry.
;
;This flag brings to light the fact that there are some circumstances when the
;VDD cannot go out and read the registry in response to the call to
;VDD_GET_DISPLAY_CONFIG or VDD_GetDisplayInfo (due to system multi-tasking
;considerations).  In this case, this flag will be set to a 1 to indicate that
;the information being returned isn't "fresh" -- that is -- it may be
;incorrect and obsolete.  The caller should respond accordingly if this flag
;is set.
;
RETURNED_DATA_IS_STALE			equ	01h
;
;MINIVDD_FAILED_TO_LOAD if set, indicates that for some reason (typically
;that the MiniVDD didn't match the chipset installed in the machine), the
;MiniVDD didn't load.  Callers can examine this flag and act accordingly.
;
MINIVDD_FAILED_TO_LOAD			equ	02h
;
;MINIVDD_CHIP_ID_DIDNT_MATCH means that although the MiniVDD did load
;successfully, when the ChipID that the MiniVDD calculated was compared
;against the value saved in the registry, they didn't match.  An example of
;when this would happen is when the user is happily using an S3-911 card
;and then decides to upgrade his display card to an S3-864.  Since both
;cards use S3.VXD, the MiniVDD will load, however, since the card model
;is different, the VDD will return a defect to configuration manager and
;set this flag.  Callers of the GET_DISPLAY_CONFIG functions can use this
;flag to take appropriate actions to make sure that the user gets his
;configuration correct.
;
MINIVDD_CHIP_ID_DIDNT_MATCH		equ	04h
;
;REGISTRY_BPP_NOT_VALID if set, means that we failed to obtain the BPP value
;from the registry when the VDD tried to read it.
;
REGISTRY_BPP_NOT_VALID			equ	08h
;
;REGISTRY_RESOLUTION_NOT_VALID if set, means that we failed to obtain the
;resolution value from the registry when the VDD tried to read it.
;
REGISTRY_RESOLUTION_NOT_VALID		equ	10h
;
;REGISTRY_DPI_NOT_VALID if set, means that we failed to obtain the
;DPI value from the registry when the VDD tried to read it.
;
REGISTRY_DPI_NOT_VALID			equ	20h
;
;MONITOR_DEVNODE_NOT_ACTIVE is set if someone tries to make a call to the
;GET_DISPLAY_CONFIG function before the monitor DevNode has been created.
;This is certainly not fatal by any means.  It simply means that the
;monitor refresh rate info in the DISPLAYINFO data structure is totally
;invalid!
;
MONITOR_DEVNODE_NOT_ACTIVE		equ	40h
;
;MONITOR_INFO_NOT_VALID indicates that something within the code which
;retrieves and calculates the refresh rate data has failed.  This indicates
;that the values in diRefreshRateMax through diVertSyncPolarity are not
;valid and could contain random data.
;
MONITOR_INFO_NOT_VALID			equ	80h
;
;MONITOR_INFO_DISABLED_BY_USER indicates that the either the RefreshRate=
;string in SYSTEM.INI had a negative number in it or that the string in
;the display's software key (HKEY_CURRENT_CONFIG\Display\Settings\RefreshRate)
;was 0 or a negative number.
;
MONITOR_INFO_DISABLED_BY_USER		equ	100h
;
;REFRESH_RATE_MAX_ONLY indicates that there was no diLowHorz, diHighHorz,
;diLowVert, diHighVert, or sync polarity data in the registry.	The
;value returned in diRefreshRateMax is the only refresh rate data that
;we have available.  This was derived either from RefreshRate= in SYSTEM.INI
;or the HKEY_CURRENT_CONFIG\Display\Settings\RefreshRate = string in the
;registry.
;
REFRESH_RATE_MAX_ONLY			equ	200h
;
;CARD_VDD_LOADED_OK indicates that a second MiniVDD (which is useful for
;display card manufacturers wishing to extend the capabilities of the chip level
;MiniVDD's ) has successfully been loaded and initialized.
;
CARD_VDD_LOADED_OK			equ	400h
