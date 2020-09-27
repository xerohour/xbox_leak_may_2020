;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Copyright (C) 1998-1999, Nvidia Corporation
;
; File:         gngdi.inc
;
; Purpose:      This file holds the interface to the generic
;               service routines.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

; gnblt.asm
EXTERNDEF       GN_BitBltPunt:NEAR
EXTERNDEF       GN_BitBltFail:NEAR
EXTERNDEF       GN_BitBltSuccess:NEAR

; gnclrblt.asm
EXTERNDEF       GN_BitBltSrcColorDstVram:NEAR
EXTERNDEF       GN_StretchBltSrcColorDstVram:NEAR

; gninit.asm
EXTERNDEF       wNVTEXT32_Alias_In_NVTEXT32:WORD
EXTERNDEF       wNVTEXT16_Alias_In_NVTEXT32:WORD
EXTERNDEF       wFlatDataSel:WORD
EXTERNDEF	GN_LoadDllAndGetExports:FAR16
CLOSESEG _NVTEXT32
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
OPENSEG  _NVTEXT16, USE16, CODE
; gninit.asm
EXTERNDEF       wNVTEXT32_Alias_In_NVTEXT16:WORD
EXTERNDEF       wNVTEXT16_Alias_In_NVTEXT16:WORD
EXTERNDEF       wFlatDataSel_In_NVTEXT16:WORD

; gnthk16.asm
EXTERNDEF       wBreak16:WORD
EXTERNDEF       GENERIC_OutputDebugString:FAR16

; gnutil.asm
EXTERNDEF       GN_GetDeviceLocalBasePath:FAR16
EXTERNDEF       GN_AllocSelectorAndMapIt:FAR16
EXTERNDEF       GN_UnmapAndFreeSelector:FAR16
EXTERNDEF       GN_GetSelectorBase:FAR16
EXTERNDEF       GN_MemoryAlloc:FAR16
EXTERNDEF       GN_MemoryFree:FAR16
EXTERNDEF       GN_GetSelectorBase:FAR16
EXTERNDEF       GN_AllocSelectorAndNullIt:FAR16
EXTERNDEF       GN_AllocSelectorAndMapIt:FAR16
EXTERNDEF       GN_UnmapAndFreeSelector:FAR16
EXTERNDEF       GN_CheckInterlacedStereoConnection:FAR16
CLOSESEG _NVTEXT16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Now the Kernel exports
EXTERNDEF       AllocCSToDSAlias:FAR16
EXTERNDEF       GetSelectorBase:FAR16
EXTERNDEF       SetSelectorBase:FAR16
EXTERNDEF       SetSelectorLimit:FAR16
EXTERNDEF       SetLastError:FAR16
EXTERNDEF       SelectorAccessRights:FAR16
EXTERNDEF       FreeSelector:FAR16
EXTERNDEF       GetModuleHandle:FAR16
EXTERNDEF       GetProcAddress:FAR16
EXTERNDEF       GlobalAlloc:FAR16
EXTERNDEF       GlobalFree:FAR16
EXTERNDEF       GlobalHandle:FAR16
EXTERNDEF       GlobalLock:FAR16
EXTERNDEF       GlobalUnlock:FAR16
EXTERNDEF       LoadLibrary:FAR16
EXTERNDEF       FreeLibrary:FAR16
EXTERNDEF       RegOpenKey:FAR16
EXTERNDEF       RegCloseKey:FAR16
EXTERNDEF       RegEnumKey:FAR16
EXTERNDEF       RegQueryValueEx:FAR16
EXTERNDEF       RegCreateKey:FAR16
EXTERNDEF       RegSetValueEx:FAR16
EXTERNDEF       RegDeleteValue:FAR16
EXTERNDEF       OutputDebugString:FAR16
EXTERNDEF       GetVersionEx:FAR16
EXTERNDEF       IsBadReadPtr:FAR16
EXTERNDEF       IsBadWritePtr:FAR16

; A few things for the Registry functions so I don't have to include
; all the windows header files.
MAX_KEY_LEN             TEXTEQU <80H>

HKEY_CLASSES_ROOT       TEXTEQU <80000000H>
HKEY_CURRENT_USER       TEXTEQU <80000001H>
HKEY_LOCAL_MACHINE      TEXTEQU <80000002H>
HKEY_USERS              TEXTEQU <80000003H>
HKEY_PERFORMANCE_DATA   TEXTEQU <80000004H>
HKEY_CURRENT_CONFIG     TEXTEQU <80000005H>
HKEY_DYN_DATA           TEXTEQU <80000006H>
ERROR_SUCCESS           TEXTEQU <00H>

REG_NONE                TEXTEQU <000000000h>
REG_SZ                  TEXTEQU <000000001h>
REG_EXPAND_SZ           TEXTEQU <000000002h>
REG_BINARY              TEXTEQU <000000003h>
REG_DWORD               TEXTEQU <000000004h>
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
MSG_PREMODESET          	TEXTEQU <00000000H>
MSG_POSTMODESET         	TEXTEQU <00000001H>
MSG_DIRECTX_ENUM		TEXTEQU	<00000002H>
MSG_DIRECTXON           	TEXTEQU <00000003H>
MSG_DIRECTXOFF          	TEXTEQU <00000004H>
MSG_DOSON               	TEXTEQU <00000005H>
MSG_DOSOFF              	TEXTEQU <00000006H>
MSG_DISABLE             	TEXTEQU <00000007H>
MSG_LOGDEVMOVED         	TEXTEQU <00000008H>
MSG_DEVICESREMAPPED		TEXTEQU	<00000009H>
MSG_DISPLAY_DRIVER_UNLOADING	TEXTEQU	<0000000AH>
MSG_POWERON             	TEXTEQU <0000000BH>
MSG_POWEROFF            	TEXTEQU <0000000CH>

; MSG_PREMODESET, MSG_POSTMODESET
; These two are given in the order shown above. The mode is set after
; the PREMODESET message returns and before the POSTMODESET message
; is sent. These messages are when the logical mode changes only!
; If the CRTC is being set to a different mode, but the desktop size
; is not changing, then these messages will not be sent.
; No one processing these messages should screw up the screen,
; change the pitch, etc in any way. They are just information to
; allow the various objects to set some internal state variables
; alloc and free resources. All calls that change state affecting
; the screens appearance will be outside of the message stuff,
; thus allowing for better control and serialization.

; MSG_PREMODESET
; dwData1 is a context ptr to a DIBENGINE structure which reflects the
; current logical mode - not the new mode being set. This gives everyone
; a chance to shutdown anything in the old mode before the modeset.
; dwData2 is the 16:16 ptr to the same thing.

; MSG_POSTMODESET
; dwData1 is a context ptr to a DIBENGINE structure which reflects the
; new logical mode that is being set.
; dwData2 is the 16:16 ptr to the same thing.

; MSG_DIRECTX_ENUM, MSG_DIRECTX_ON, MSG_DIRECTX_OFF
; MSG_DIRECTX_ENUM means that the system is at the very least enumerating
; all the DirectX devices and that includes issuing an escape to all the
; display drivers that power the 2D side of the device.
; MSG_DIRECTXON means that the first app has attached to the DirectX
; Context. This is when offscreen video memory should be releaseed
; for use by DirectX.
; MSG_DIRECTXOFF means that the last app has released the DirectX Context.
; Thi is when the 2D driver can reclaim offscreen video memory.
; dwData1 and dwData2 are unused.

; MSG_DOSON, MSG_DOSOFF, MSG_POWERON, MSG_POWEROFF
; dwData1 is a context ptr to a DIBENGINE structure which reflects the
; new logical mode that is being set.
; dwData2 is the 16:16 ptr to the same thing.

; MSG_DISABLE
; The device is being disabled. In general, the action here should
; be similar to a DOSON msg. In both cases, it probably means that
; the driver is temporarily losing control of the screen and the
; hardware.
; dwData1 and dwData2 are unused.

; MSG_LOGDEVMOVED
; dwData1 is the new pGNLogdevCX.
; dwData2 is the old pGNLogdevCX.

; MSG_DEVICESREMAPPED
; This message says that the hardware has remapped the device
; to head assignments. Although the generic code may not know
; exactly what this means, the idea is that things relating to
; head DAC speeds, etc, may be different.
; dwData1 is ignored
; dwData2 is ignored

; MSG_PRE_RESETTING_VIDEO_MEMORY_HEAP
; MSG_POST_RESETTING_VIDEO_MEMORY_HEAP
; This message is only sent down in catastrophic cases where we were
; unable to allocate even a 640x480x8 primary surface for one of the
; displays ona modeset. Since the code in the display driver should
; always allow us to get at least this surface size, it must be that
; somehow the heap is corrupted. In that case, we reset it so that
; it is completely clean and inform everyone so they can do any clean
; up. The pre message is sent just before the heap is purged. The post
; message is sent after a new one has been allocated. These messages
; may well be sent nested inside a MSG_PREMODESET and MSG_POSTMODESET
; pair, because it is on a modeset that we discover we can't get the
; primary surface.
; dwData1 is a ptr to the DIBENGINEHDR structure describing the
;	  current mode. Since we are probably inside a PREMODESET/
;	  POSTMODESET pair, dwData1 will describe the mode that
;	  exists BEFORE the mode change.
; dwData2 is not used

PLABEL	MACRO	name
	public	&name&
&name&	proc	near
&name&	endp
	ENDM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

