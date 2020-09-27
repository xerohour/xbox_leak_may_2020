page            ,132
title		Mini VDD Kernel Mode Video Transport (VPE ring 0) dispatch 
.386p
;
;
.xlist
include	VMM.INC
include	VMMREG.INC
include	VWIN32.INC
include 	MINIVDD.INC
include	DDKMMINI.INC
include  DEBUG.INC
include  SURFACES.INC
.list

extrn _vddGetIRQInfo:near
extrn _vddEnableIRQ:near
extrn _vddIsOurIRQ:near
extrn _vddFlipVideoPort:near
extrn _vddFlipOverlay:near
extrn _vddBobNextField:near
extrn _vddSetState:near
extrn _vddLock:near
extrn _vddSkipNextField:near
extrn _vddGetFieldPolarity:near
extrn _vddSetSkipPattern:near
extrn _vddGetCurrentAutoFlip:near
extrn _vddGetPreviousAutoFlip:near
;  these are the nvidia specific functions
extrn _vddgetStoragePointer:near
extrn _vddhandleBufferInterrupts:near
extrn _vddSetCallback:near
extrn _vddTransfer:near
extrn _vddGetTransferStatus:near
IFDEF   NV4_HW
extrn _vddSetVidTexSuf:near
ENDIF    ;NV4_HW

VxD_LOCKED_DATA_SEG
;
; Many of these functions are called at interrupt time
; So all data must be allocated in a locked (non-swappable) segment
;


;CODE
; until we have use for the context..... ignore
;extrn MMIOLinearAddrBase:dword
                   
VxD_LOCKED_DATA_ENDS

VxD_DATA_SEG
VxD_DATA_ENDS

VxD_LOCKED_CODE_SEG

;****************************************************************************
;
;   GetDDHAL
;
;   DESCRIPTION: This function fills in the function table supplied
;	by DDRAW with functions that their VxD can call fro kernel mode
;	DirectDraw support.  The MiniVDD can fill in the dwContext field
;	with a value that will always be passed back when called.
;
;   ENTRY:
;	   ESI  Ptr to DIOCParams
;		ESI+18h  Ptr to output buffer
;		ESI+1Ch  Size of output buffer
;		ESI+20h  Amount of data we write into output buffer
;
;   EXIT:
;          EAX	0 = success
;
;****************************************************************************

public  GetDDHAL
BeginProc GetDDHAL

	; get the size of the DDMINIVDDTABLE from the OutBuffer
	; compare with known size and error if there is a problem
	mov	ebx, [esi.DIOCParams.cbOutBuffer]
	cmp	ebx, SIZE _DDMINIVDDTABLE
	jl	GetDDHAL_Error

	; confirm that the input buffer has size of 4 bytes
	mov	ebx, [esi.DIOCParams.cbInBuffer]
	cmp	ebx, 4
	jl	GetDDHAL_Error


	; put address of output buffer in ebx
	mov	ebx, [esi.DIOCParams.lpvOutBuffer]

	;load context... and then the offset addresses of the various functions

	; load address of InBuffer
	mov   eax, [esi.DIOCParams.lpvInBuffer]
	; grab first U032 of InBuffer
	mov	eax,[eax]
	; store that as the context
	mov	[ebx._DDMINIVDDTABLE.dwMiniVDDContext], eax
	
	
	; load up the various function offsets
	mov	eax, OFFSET32 DDGetIRQInfo
	mov	[ebx._DDMINIVDDTABLE.vddGetIRQInfo], eax
	mov	eax, OFFSET32 DDIsOurIRQ
	mov	[ebx._DDMINIVDDTABLE.vddIsOurIRQ], eax
	mov	eax, OFFSET32 DDEnableIRQ
	mov	[ebx._DDMINIVDDTABLE.vddEnableIRQ], eax
	mov	eax, OFFSET32 DDSkipNextField
	mov	[ebx._DDMINIVDDTABLE.vddSkipNextField], eax
	mov	eax, OFFSET32 DDBobNextField
	mov	[ebx._DDMINIVDDTABLE.vddBobNextField], eax
	mov	eax, OFFSET32 DDSetState
	mov	[ebx._DDMINIVDDTABLE.vddSetState], eax
	mov	eax, OFFSET32 DDLock
	mov	[ebx._DDMINIVDDTABLE.vddLock], eax
	mov	eax, OFFSET32 DDFlipOverlay
	mov	[ebx._DDMINIVDDTABLE.vddFlipOverlay], eax
	mov	eax, OFFSET32 DDFlipVideoPort
	mov	[ebx._DDMINIVDDTABLE.vddFlipVideoPort], eax
	mov	eax, OFFSET32 DDGetFieldPolarity
	mov	[ebx._DDMINIVDDTABLE.vddGetPolarity], eax
	mov	eax, OFFSET32 DDSetSkipPattern
	mov	[ebx._DDMINIVDDTABLE.vddReserved1], eax
	mov	eax, OFFSET32 DDGetCurrentAutoflipSurface
	mov	[ebx._DDMINIVDDTABLE.vddGetCurrentAutoflip], eax
	
	mov	eax, OFFSET32 DDGetPreviousAutoflipSurface
	mov	[ebx._DDMINIVDDTABLE.vddGetPreviousAutoflip], eax
	
	mov	eax, OFFSET32 DDTransfer
	mov	[ebx._DDMINIVDDTABLE.vddTransfer], eax
	
	mov	eax, OFFSET32 DDGetTransferStatus
	mov	[ebx._DDMINIVDDTABLE.vddGetTransferStatus], eax
	
	mov	ebx, [esi.DIOCParams.lpcbBytesReturned]
	mov	eax, SIZE _DDMINIVDDTABLE
	mov	[ebx], eax

	Debug_Printf    "XXXXXXXXXXXXXXXXXXXXXX  Calling Fill the DD HAL table\n\r", eax, DEBLEVELMAX
        
; zero out the return value are return
	sub	eax, eax
	ret

GetDDHAL_Error:
;    this next one is probably left over from debugging
;    INT3
	mov	eax, 1
	ret
 
EndProc GetDDHAL

;****************************************************************************
;
;   NvFillTable
;
;   DESCRIPTION: This function will fill in a table with all of the Nvidia 
;   specific functions which allow us to work between ring3 and ring0 VPE stuff
;
;   ENTRY:
;	   ESI  Ptr to DIOCParams
;		ESI+18h  Ptr to output buffer
;		ESI+1Ch  Size of output buffer
;		ESI+20h  Amount of data we write into output buffer
;
;   EXIT:
;          EAX	0 = success
;
;****************************************************************************

public  NvFillTable
BeginProc NvFillTable

	; get the size of the NVVDDPROCTABLE from the OutBuffer
	; compare with known size and error if there is a problem
	
	mov	ebx, [esi.DIOCParams.cbOutBuffer]
	cmp	ebx, SIZE _NVVDDPROCTABLE
	jl	NvFillTable_Error

	; confirm that the input buffer has size of 4 bytes
	mov	ebx, [esi.DIOCParams.cbInBuffer]
	cmp	ebx, 4
	jl	NvFillTable_Error

	; put address of output buffer in ebx
	mov	ebx, [esi.DIOCParams.lpvOutBuffer]

	;load context... and then the offset addresses of the various functions

	; load address of InBuffer
	mov   eax, [esi.DIOCParams.lpvInBuffer]

	; grab first U032 of InBuffer  this is the pointer to the direct draw drivers
	mov	eax,[eax]

	; store that as the context
	mov	[ebx._NVVDDPROCTABLE.dwPDDDriver], eax
	
	
	; load up the various function offsets
	mov	eax, OFFSET32 VDhandleBufferInterrupts
	mov	[ebx._NVVDDPROCTABLE.handleBufferInterrupts], eax

	mov	eax, OFFSET32 VDgetStoragePointer
	mov	[ebx._NVVDDPROCTABLE.getStoragePointer], eax

; zero out the return value are return
	sub	eax, eax
	ret

NvFillTable_Error:
	mov	eax, 1
	ret
 

EndProc NvFillTable

;****************************************************************************
;
;   NVSetVidTexSuf
;
;   DESCRIPTION: This function records a pointer passed in from an APP.  Pointer points to some surface data
;
;   ENTRY:
;	   ESI  Ptr to DIOCParams
;		ESI+18h  Ptr to output buffer
;		ESI+1Ch  Size of output buffer
;		ESI+20h  Amount of data we write into output buffer
;
;   EXIT:
;          EAX	0 = success
;
;****************************************************************************

public  NVSetVidTexSuf
BeginProc NVSetVidTexSuf
	
	mov	ebx, [esi.DIOCParams.cbOutBuffer]
	cmp	ebx, 4
	jl	NVSetVidTexSuf_Error

	; confirm that the input buffer has size of 4 bytes
	mov	ebx, [esi.DIOCParams.cbInBuffer]
	cmp	ebx, 4
	jl	NVSetVidTexSuf_Error

	; put address of output buffer in ebx
	mov	ebx, [esi.DIOCParams.lpvOutBuffer]

	; load address of InBuffer
	mov   eax, [esi.DIOCParams.lpvInBuffer]

IFDEF   NV4_HW
    push    ebx		; preserve
    push    ecx		; preserve
    push    edx		; preserve	
    push    eax	; struct pointer
    push    ebx    ; vmm context
;    call    _vddSetVidTexSuf
    pop    ebx    ; vmm context
    pop    ebx    ; vmm context
    
	 pop     edx		; preserve
    pop     ecx		; preserve
    pop     ebx		; preserve

	; eax contain return value... no big deal we already filled output buffer
	 
; zero out the return value are return
	sub	eax, eax
	ret
ENDIF  ;  NV4_HW


NVSetVidTexSuf_Error:
	mov	eax, 1
	ret
 
EndProc NVSetVidTexSuf


;****************************************************************************
;
;   NVGetSetCallback
;
;   DESCRIPTION: This function fills in a record about callbacks
;
;   ENTRY:
;	   ESI  Ptr to DIOCParams
;		ESI+18h  Ptr to output buffer
;		ESI+1Ch  Size of output buffer
;		ESI+20h  Amount of data we write into output buffer
;
;   EXIT:
;          EAX	0 = success
;
;****************************************************************************

public  NVGetSetCallback
BeginProc NVGetSetCallback
	
	mov	ebx, [esi.DIOCParams.cbOutBuffer]
	cmp	ebx, 4
	jl	NvGetSetCallBack_Error

	; confirm that the input buffer has size of 4 bytes
	mov	ebx, [esi.DIOCParams.cbInBuffer]
	cmp	ebx, 4
	jl	NvGetSetCallBack_Error

	; put address of output buffer in ebx
	mov	ebx, [esi.DIOCParams.lpvOutBuffer]

	; load address of InBuffer
	mov   eax, [esi.DIOCParams.lpvInBuffer]

	mov	eax,  OFFSET32 _vddSetCallback
	mov	[ebx], eax

	; load address of InBuffer
	mov   eax, [esi.DIOCParams.lpvInBuffer]
    
	; eax contain return value... no big deal we already filled output buffer
	 
; zero out the return value are return
	sub	eax, eax
	ret

NvGetSetCallBack_Error:
	mov	eax, 1
	ret
 
EndProc NVGetSetCallback

public  NVSetCallback

BeginProc NVSetCallback
	
    call    _vddSetCallback
    ret

EndProc NVSetCallback

BeginProc VDhandleBufferInterrupts, RARE
    push    ebx		; preserve
    push    ecx		; preserve
    push    edx		; preserve	
    push    edi	; struct pointer
    push    esi	; struct pointer
    push    eax    ; vmm context
    call    _vddhandleBufferInterrupts
    pop     edi	; balance eax
    pop     esi	; struct pointer
    pop     edi	; struct pointer	
    pop     edx		; preserve
    pop     ecx		; preserve
    pop     ebx		; preserve
    ret

EndProc VDhandleBufferInterrupts

BeginProc VDgetStoragePointer, RARE
    push    ebx		; preserve
    push    ecx		; preserve
    push    edx		; preserve	
    push    edi	; struct pointer
    push    esi	; struct pointer
    push    eax    ; vmm context
    call    _vddgetStoragePointer
    pop     edi	; balance eax
    pop     esi	; struct pointer
    pop     edi	; struct pointer	
    pop     edx		; preserve
    pop     ecx		; preserve
    pop     ebx		; preserve
    ret

EndProc VDgetStoragePointer

;****************************************************************************
;
;   DDGetIRQInfo
;
;   DESCRIPTION: If the Mini VDD is already managing the IRQ, this
;          function returns that information; otherwise, it returns the
;          IRQ number assigned to the device so DDraw can manage the IRQ.
;
;          The returning the IRQ number, it is important that it get the
;          value assigned by the Config Manager rather than simply get
;          the value from the hardware (since it can be remapped by PCI).
;
;   ENTRY:
;	   ESI	NULL
;	   EDI  LPDDGETIRQINFO
;		    DWORD dwSize;
;		    DWORD dwFlags;
;		    DWORD dwIRQNum;
;
;   EXIT:
;          EAX	0 = success, 1 = error
;
;****************************************************************************
BeginProc DDGetIRQInfo, RARE
    push    ebx		; preserve
    push    ecx		; preserve
    push    edx		; preserve	
    push    edi	; struct pointer
    push    esi	; struct pointer
    push    eax    ; vmm context
    call    _vddGetIRQInfo
    pop     edi	; balance eax
    pop     esi	; struct pointer
    pop     edi	; struct pointer	
    pop     edx		; preserve
    pop     ecx		; preserve
    pop     ebx		; preserve
    ret

EndProc DDGetIRQInfo



;****************************************************************************
;
;   DDEnableIRQ
;
;   DESCRIPTION: Notifies the Mini VDD which IRQs should be enabled.  If
;          a previously enabled IRQ is not specified in this call,
;          it should be disabled.
;
;   ENTRY:
;	   ESI	LPDDENABLEIRQINFO
;		    DWORD dwSize
;		    DWORD dwIRQSources
;		    DWORD dwLine
;		    DWORD IRQCallback
;		    DWORD dwContext
;	   EDI  NULL
;
;   EXIT:
;          EAX	0 = success, 1 = error
;
;****************************************************************************
BeginProc DDEnableIRQ, RARE
    push    ebx		; preserve
    push    ecx		; preserve
    push    edx		; preserve	
    push    edi	; struct pointer
    push    esi	; struct pointer
    push    eax    ; vmm context
    call    _vddEnableIRQ
    pop     edi	; balance eax
    pop     esi	; struct pointer
    pop     edi	; struct pointer	
    pop     edx		; preserve
    pop     ecx		; preserve
    pop     ebx		; preserve
    ret

EndProc DDEnableIRQ

;****************************************************************************
;
;   DDIsOurIRQ
;
;   DESCRIPTION: Called when the VDD's IRQ handled is triggered.  This
;          determines if the IRQ was caused by our VGA and if so, it
;          clears the IRQ and returns which event(s) generated the IRQ.
;
;   ENTRY:
;	   ESI	NULL
;
;   EXIT:
;	   EDI  IRQ source flags
;          EAX	0 = success, 1 = error
;
;****************************************************************************
BeginProc DDIsOurIRQ, High_Freq
    push    ebx		; preserve
    push    ecx		; preserve
    push    edx		; preserve	
    push    edi	; struct pointer
    push    esi	; struct pointer
    push    eax    ; vmm context
    call    _vddIsOurIRQ
    pop     edi	; balance eax
    pop     esi	; struct pointer
    pop     edi	; struct pointer	
    pop     edx		; preserve
    pop     ecx		; preserve
    pop     ebx		; preserve
    ret

EndProc DDIsOurIRQ


;****************************************************************************
;
;   DDFlipVideoPort
;
;   DESCRIPTION: Flips the video port to the target surface.
;
;   ENTRY:
;	   ESI	LPDDFLIPVIDEOPORTINFO
;		    DWORD dwSize
;		    LPDDVIDEOPORTDATA video port info
;		    LPDDSURFACEDATA current surface
;		    LPDDSURFACEDATA target surface
;		    DWORD dwFlipVPFlags
;	   EDI  NULL
;
;   EXIT:
;          EAX	0 = success, 1 = error
;
;****************************************************************************

BeginProc DDFlipVideoPort, High_Freq
    push    ebx		; preserve
    push    ecx		; preserve
    push    edx		; preserve	
    push    edi	; struct pointer
    push    esi	; struct pointer
    push    eax    ; vmm context
    call    _vddFlipVideoPort
    pop     edi	; balance eax
    pop     esi	; struct pointer
    pop     edi	; struct pointer	
    pop     edx		; preserve
    pop     ecx		; preserve
    pop     ebx		; preserve
    ret

EndProc DDFlipVideoPort

;****************************************************************************
;
;   DDFlipOverlay
;
;   DESCRIPTION: Flips the overlay to the target surface.
;  
;   ENTRY:
;	   ESI	LPDDFLIPOVERLAYINFO
;		    DWORD dwSize
;		    LPDDSURFACEDATA current surface
;		    LPDDSURFACEDATA target surface
;		    DWORD flags
;	   EDI  NULL
;
;   EXIT:
;          EAX	0 = success, 1 = error
;
;****************************************************************************
BeginProc DDFlipOverlay, High_Freq
    push    ebx		; preserve
    push    ecx		; preserve
    push    edx		; preserve	
    push    edi	; dummy
    push    esi	; struct pointer
    push    eax    ; vmm context
    call    _vddFlipOverlay
    pop     edi	; balance eax
    pop     esi	; struct pointer
    pop     edi	; dummy	
    pop     edx		; preserve
    pop     ecx		; preserve
    pop     ebx		; preserve
    ret

EndProc DDFlipOverlay


;****************************************************************************
;
;   DDBobNextInterleavedEvenOverlayField
;
;   DESCRIPTION: Called when "bob" is used and a VPORT VSYNC occurs that does
;       not cause a flip to occur (e.g. bobbing while interleaved).  When
;       bobbing, the overlay must adjust itself on every VSYNC, so this
;       function notifies it of the VSYNCs that it doesn't already know
;       about (e.g. VSYNCs that trigger a flip to occur).
;
;   ENTRY:
;	   ESI	LPDDBOBINFO
;		    DWORD dwSize
;		    LPDDSURFACE lpSurface
;	   EDI  NULL
;
;   EXIT:
;          EAX	0 = success, 1 = error
;
;****************************************************************************
BeginProc DDBobNextField, High_Freq
    push    ebx		; preserve
    push    ecx		; preserve
    push    edx		; preserve	
    push    edi	; dummy
    push    esi	; struct pointer
    push    eax    ; vmm context
    call    _vddBobNextField
    pop     edi	; balance eax
    pop     esi	; struct pointer
    pop     edi	; dummy	
    pop     edx		; preserve
    pop     ecx		; preserve
    pop     ebx		; preserve
    ret

EndProc DDBobNextField


;****************************************************************************
;
;   DDSetState
;
;   DESCRIPTION: Called when the client wants to switch from bob to weave.
;	The overlay flags indicate which state to use. Only called for interleaved
;   surfaces.
;
;	NOTE: When this is called, the specified surface may not be
;	displaying the overlay (due to a flip).  Instead of failing
;	the call, change the bob/weave state for the overlay that would
;	be used if the overlay was flipped again to the specified surface.
;
;   ENTRY:
;	   ESI	LPDDSTATEININFO
;		    DWORD dwSize
;		    LPDDSURFACEDATA overlay surface
;	   EDI  LPDDSTATEOUTINFO
;		    DWORD dwSize
;		    DWORD dwSoftwareAutoflip
;		    DWORD dwSurfaceIndex        ; Return Current hardware autoflip
;
;
;   EXIT:
;          EAX	0 = success, 1 = error
;
;****************************************************************************
BeginProc DDSetState, High_Freq
    push    ebx		; preserve
    push    ecx		; preserve
    push    edx		; preserve	
    push    edi	; struct pointer
    push    esi	; struct pointer
    push    eax    ; vmm context
    call    _vddSetState
    pop     edi	; balance eax
    pop     esi	; struct pointer
    pop     edi	; struct pointer	
    pop     edx		; preserve
    pop     ecx		; preserve
    pop     ebx		; preserve
    ret

EndProc DDSetState


;****************************************************************************
;
;   DDLock
;
;   DESCRIPTION: Called when the client wants to lock the surface to
;	access the frame buffer. The driver doens't have to do anything,
;       but it can if it needs to.
;
;   ENTRY:
;	   ESI	LPDDLOCKININFO
;		    DWORD dwSize
;		    LPDDSURFACEDATA surface
;	   EDI  LPDDLOCKOUTINFO
;		    DWORD dwSize
;		    DWORD Pointer to a pointer to the surface
;
;   EXIT:
;          EAX	0 = success, 1 = error
;
;****************************************************************************

BeginProc DDLock, High_Freq
    push    ebx		; preserve
    push    ecx		; preserve
    push    edx		; preserve	
    push    edi	; struct pointer
    push    esi	; struct pointer
    push    eax    ; vmm context
    call    _vddLock
    pop     edi	; balance eax
    pop     esi	; struct pointer
    pop     edi	; struct pointer	
    pop     edx		; preserve
    pop     ecx		; preserve
    pop     ebx		; preserve
    ret

EndProc DDLock


;****************************************************************************
;
;   DDSkipNextVideoPortField
;
;   DESCRIPTION: Called when they want to skip the next field, usually
;       to undo a 3:2 pulldown but also for decreasing the frame rate.
;       The driver should not lose the VBI lines if dwVBIHeight contains
;       a valid value.
;
;   ENTRY:
;	   ESI	LPDDSKIPINFO
;		    DWORD dwSize
;		    LPDDVIDEOPORTDATA video port
;	   EDI  NULL
;
;   EXIT:
;          EAX	0 = success, 1 = error
;
;****************************************************************************
BeginProc DDSkipNextField, High_Freq
    push    ebx		; preserve
    push    ecx		; preserve
    push    edx		; preserve	
    push    edi	; struct pointer
    push    esi	; struct pointer
    push    eax    ; vmm context
    call    _vddSkipNextField
    pop     edi	; balance eax
    pop     esi	; struct pointer
    pop     edi	; struct pointer	
    pop     edx		; preserve
    pop     ecx		; preserve
    pop     ebx		; preserve
    ret

EndProc DDSkipNextField

;****************************************************************************
;
;   DDGetFieldPolarity
;
;   DESCRIPTION: Returns the polarity of the current field being written
;	to the specified video port.
;
;   ENTRY:
;	   ESI	LPDDPOLARITYININFO
;		    DWORD dwSize
;		    LPDDVIDEOPORTDATA
;	   EDI  LPDDPOLARITYOUTINFO
;		    DWORD dwSize
;		    DWORD bPolority (even field = TRUE, odd field = FALSE)
;
;   EXIT:
;          EAX	0 = success, 1 = error
;          ECX  0 = odd,     1 = even
;
;****************************************************************************

BeginProc DDGetFieldPolarity, High_Freq
    push    ebx		; preserve
    push    ecx		; preserve
    push    edx		; preserve	
    push    edi	; struct pointer
    push    esi	; struct pointer
    push    eax    ; vmm context
    call    _vddGetFieldPolarity
    pop     edi	; balance eax
    pop     esi	; struct pointer
    pop     edi	; struct pointer	
    pop     edx		; preserve
    pop     ecx		; preserve
    pop     ebx		; preserve
	 mov		ecx,eax	;  more the value to ECX
	 xor     eax,eax	;  zero out the return value since we always return sucess
	 ret

EndProc DDGetFieldPolarity

;****************************************************************************
;
;   DDSetSkipPattern
;
;   DESCRIPTION: Sets the skip pattern in hardware
;
;   ENTRY:
;	   ESI	LPDDSETSKIPINFO
;		    DWORD 		dwSize
;		    LPDDVIDEOPORTDATA	lpVideoPortData
;		    DWORD		dwPattern
;		    DWORD		dwPatternSize
;	   EDI  LPDDPOLARITYOUTINFO
;
;   EXIT:
;          EAX	0 = success, 1 = error
;
;****************************************************************************

BeginProc DDSetSkipPattern, High_Freq
    push    ebx		; preserve
    push    ecx		; preserve
    push    edx		; preserve	
    push    edi	; struct pointer
    push    esi	; struct pointer
    push    eax    ; vmm context
    call    _vddSetSkipPattern
    pop     edi	; balance eax
    pop     esi	; struct pointer
    pop     edi	; struct pointer	
    pop     edx		; preserve
    pop     ecx		; preserve
    pop     ebx		; preserve
    ret

EndProc DDSetSkipPattern



;****************************************************************************
;
;   DDGetCurrentAutoflipSurface
;
;   DESCRIPTION: Returns the current surface receiving data from the
;	video port while autoflipping is taking palce.  Only called when
;   hardware autoflipping.
;
;   ENTRY:
;	   ESI	LPDDGETAUTOFLIPINFO
;		    DWORD 		dwSize
;	   EDI	LPDDGETAUTOFLIPINFO
;		    DWORD 		dwSize
;		    DWORD		dwSurfaceIndex
;
;   EXIT:
;          EAX	0 = success, 1 = error
;
;****************************************************************************

BeginProc DDGetCurrentAutoflipSurface, High_Freq
    push    ebx		; preserve
    push    ecx		; preserve
    push    edx		; preserve	
    push    edi	; struct pointer
    push    esi	; struct pointer
    push    eax    ; vmm context
    call    _vddGetCurrentAutoflip
    pop     edi	; balance eax
    pop     esi	; struct pointer
    pop     edi	; struct pointer	
    pop     edx		; preserve
    pop     ecx		; preserve
    pop     ebx		; preserve
    ret

EndProc DDGetCurrentAutoflipSurface
;****************************************************************************
;
;   DDGetPreviousAutoflipSurface
;
;   DESCRIPTION: Returns the surface that received the data from the
;	previous field of video port while autoflipping is taking palce. Only
;   called for hardware autoflipping.
;
;   ENTRY:
;	   ESI	LPDDGETAUTOFLIPINFO
;		    DWORD 		dwSize
;	   EDI	LPDDGETAUTOFLIPINFO
;		    DWORD 		dwSize
;		    DWORD		dwSurfaceIndex
;
;   EXIT:
;          EAX	0 = success, 1 = error
;
;****************************************************************************

BeginProc DDGetPreviousAutoflipSurface, High_Freq
    push    ebx		; preserve
    push    ecx		; preserve
    push    edx		; preserve	
    push    edi	; struct pointer
    push    esi	; struct pointer
    push    eax    ; vmm context
    call    _vddGetPreviousAutoFlip
    pop     edi	; balance eax
    pop     esi	; struct pointer
    pop     edi	; struct pointer	
    pop     edx		; preserve
    pop     ecx		; preserve
    pop     ebx		; preserve
    ret

EndProc DDGetPreviousAutoflipSurface

;****************************************************************************
;
;   DDTransfer
;
;   DESCRIPTION: Returns the surface that received the data from the
;	previous field of video port while autoflipping is taking palce. Only
;   called for hardware autoflipping.
;
;   ENTRY:
;	   ESI	DDTRANSFERININFO
;
;	   EDI	DDTRANSFEROUTINFO
;
;   EXIT:
;          EAX	0 = success, 1 = error
;
;****************************************************************************

BeginProc DDTransfer, High_Freq
    push    ebx		; preserve
    push    ecx		; preserve
    push    edx		; preserve	
    push    edi	; struct pointer
    push    esi	; struct pointer
    push    eax    ; vmm context
    call    _vddTransfer
    pop     edi	; balance eax
    pop     esi	; struct pointer
    pop     edi	; struct pointer	
    pop     edx		; preserve
    pop     ecx		; preserve
    pop     ebx		; preserve
    ret

EndProc DDTransfer


;****************************************************************************
;
;   DDGetTransferStatus
;
;
;   ENTRY:
;	   ESI	DDGETTRANSFERSTATUSOUTINFO
;
;	   EDI  NULL
;
;   EXIT:
;          EAX	0 = success, 1 = error
;
;****************************************************************************

BeginProc DDGetTransferStatus, High_Freq
    push    ebx		; preserve
    push    ecx		; preserve
    push    edx		; preserve	
    push    edi	; struct pointer
    push    esi	; struct pointer
    push    eax    ; vmm context
    call    _vddGetTransferStatus
    pop     edi	; balance eax
    pop     esi	; struct pointer
    pop     edi	; struct pointer	
    pop     edx		; preserve
    pop     ecx		; preserve
    pop     ebx		; preserve
    ret

EndProc DDGetTransferStatus


public _vwin32SetWin32Event
public _vwin32ResetWin32Event
public _vwin32WaitSingleObject
public _vwin32TimeSliceSleep

BeginProc _vwin32SetWin32Event, CCALL, PUBLIC

ArgVar hEvent, DWORD

		EnterProc

    
    VMMCall Test_Sys_VM_Handle              ;is it the system VM?
    ; if not the system vm don't call this func
    jnz    NotGoingtoHappen
    
    ; 
		pushad
    
		mov		eax, [hEvent]

		VxDCall _VWIN32_SetWin32Event 

		popad

NotGoingtoHappen:
		LeaveProc
		return

EndProc _vwin32SetWin32Event


BeginProc _vwin32ResetWin32Event, CCALL, PUBLIC

ArgVar hEvent, DWORD

		EnterProc

		pushad
    
		; It already comes in EAX
        ;mov		eax, [hEvent]

		VxDCall _VWIN32_ResetWin32Event 

		popad

		LeaveProc
		return

EndProc _vwin32ResetWin32Event

BeginProc _vwin32WaitSingleObject, CCALL, PUBLIC

ArgVar hEvent, DWORD
ArgVar timeOut, DWORD
ArgVar dwFlag, DWORD

		EnterProc

		pushad
    
    mov		eax, [dwFlag]
    mov		edx, [timeOut]
    mov		ebx, [hEvent]

		VxDCall _VWIN32_WaitSingleObject 
    
    popad

		LeaveProc
		return

EndProc _vwin32WaitSingleObject

BeginProc _vwin32TimeSliceSleep, CCALL, PUBLIC

ArgVar dwTimeOut, DWORD

		EnterProc

		pushad
    
    mov		eax, [dwTimeOut]
    
		VMMCall Time_Slice_Sleep 
    
    popad

		LeaveProc
		return

EndProc _vwin32TimeSliceSleep


VxD_LOCKED_CODE_ENDS
end
