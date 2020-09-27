;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         misc.asm
;
; Purpose:      This file holds a few miscellaneous run-time functions.
;==============================================================================
.586
include macros.dat
include gdidefs.inc
include dibeng.inc
include gngdi.inc
include hwgdi.inc
include hwmacs.inc
include nvcm.inc
include nvos.inc
include nvmisc.inc
include sysdata.inc
include nv32.inc
include hwlogdev.inc
include hwboard.inc
include hwcrtc.inc

IFDEF   DEBUG
PLACE_DST_BITMAP_IN_SCREEN_CORNER       TEXTEQU <1>
ELSE
PLACE_DST_BITMAP_IN_SCREEN_CORNER       TEXTEQU <0>
ENDIF

IF  (PLACE_DST_BITMAP_IN_SCREEN_CORNER NE 0)
PANIT   MACRO   dBase,dPitch
        LOCAL   skip

        cmp     cs:[dwAlignOffscreenBitmap],0
        je      skip
        push    edx
        push    ecx
        mov     edx,dBase
        mov     ecx,dPitch
        call    AlignOffscreenBitmap
        pop     ecx
        pop     edx
skip:
ENDM
ELSE
PANIT   MACRO   dBase,dPitch
ENDM
ENDIF

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

IF  (PLACE_DST_BITMAP_IN_SCREEN_CORNER NE 0)
PUBLIC  dwAlignOffscreenBitmap
dwAlignOffscreenBitmap  DD      0
ENDIF

; This is a HACK! Because some of the objects don't use their own
; SetColorFormat field, but instead use what is in the their embedded
; CONTEXT_SURFACES_2D object, we have to reset the ColorFormat for the
; CONTEXT_SURFACES_2D every time! Unh. The color format is dependent
; upon the logical device. If we have to pass all this information
; into the SetSrcBaseAndPitchToXX routines, the caller gets tongue-
; tied with all the parameters and values to pass. The driver loads
; once for each logical device, so we just remember the correct
; ColorFormat Value here in the code segment. This vairable must
; be updated on modesets to this logical device.
PUBLIC  dwContextSurface2DColorFormat
dwContextSurface2DColorFormat   DD      NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8

;==============================================================================
;
; Function:     SetDstBaseAndPitchToDsEdi
;
; Purpose:      This function sets the dst base and pitch in the
;               ContextSurfaces2d object to the values passed in.
;
; Arguments:
;               ds      Context Selector
;               edi     push buffer ptr
;
; Returns:      None
;
; Preserve:     all registers except edi should be adjusted as
;               appropriate to place some commands in the buffer
;==============================================================================
DECPROC SetDstBaseAndPitchToDsEdi, PASCAL, FRAME, NEAR
PARMD   pHWBoardCXOffset
PARMD   dwDstBase
PARMD   dwDstPitch
OPENPROC
        PUSHR   eax,esi
        mov     esi,pHWBoardCXOffset
        CHECKDMASPACE ds,esi,edi,14H
        mov     eax,cs:[dwContextSurface2DColorFormat]
        mov     dword ptr ds:[edi + 00H],CHNRUN(ContextSurfaces2d.SetColorFormat,4)
        mov     ds:[edi + 04H],eax
        mov     eax,dwDstPitch
        shl     eax,10H
        mov     ax,word ptr ds:[esi].HWBOARDCX.dwContextSurface2dDstSrcPitch
        mov     ds:[edi + 08H],eax
        mov     ds:[esi].HWBOARDCX.dwContextSurface2dDstSrcPitch,eax
        mov     eax,ds:[esi].HWBOARDCX.dwContextSurface2dSrcOffset
        mov     ds:[edi + 0CH],eax
        mov     eax,dwDstBase
        mov     ds:[edi + 10H],eax
        mov     ds:[esi].HWBOARDCX.dwContextSurface2dDstOffset,eax
        add     edi,14H
        PANIT   dwDstBase,dwDstPitch
        POPR    eax,esi
CLOSEPROC


;==============================================================================
;
; Function:     SetDstBaseAndPitchToEsEdi
;
; Purpose:      This function sets the dst base and pitch in the
;               ContextSurfaces2d object to the values passed in.
;
; Arguments:
;               es      Context Selector
;               edi     push buffer ptr
;
; Returns:      None
;
; Preserve:     all registers except edi should be adjusted as
;               appropriate to place some commands in the buffer
;==============================================================================
DECPROC SetDstBaseAndPitchToEsEdi, PASCAL, FRAME, NEAR
PARMD   pHWBoardCXOffset
PARMD   dwDstBase
PARMD   dwDstPitch
OPENPROC
        PUSHR   eax,esi
        mov     esi,pHWBoardCXOffset
        CHECKDMASPACE es,esi,edi,14H
        mov     eax,cs:[dwContextSurface2DColorFormat]
        mov     dword ptr es:[edi + 00H],CHNRUN(ContextSurfaces2d.SetColorFormat,4)
        mov     es:[edi + 04H],eax
        mov     eax,dwDstPitch
        shl     eax,10H
        mov     ax,word ptr es:[esi].HWBOARDCX.dwContextSurface2dDstSrcPitch
        mov     es:[edi + 08H],eax
        mov     es:[esi].HWBOARDCX.dwContextSurface2dDstSrcPitch,eax
        mov     eax,es:[esi].HWBOARDCX.dwContextSurface2dSrcOffset
        mov     es:[edi + 0CH],eax
        mov     eax,dwDstBase
        mov     es:[edi + 10H],eax
        mov     es:[esi].HWBOARDCX.dwContextSurface2dDstOffset,eax
        add     edi,14H
        PANIT   dwDstBase,dwDstPitch
        POPR    eax,esi
CLOSEPROC


;==============================================================================
;
; Function:     WaitUntilOtherContextsIdle
;
; Purpose:      This function will wait until any other graphics
;               contexts DMA pushers are idle.
;
; Arguments:
;               None
;
; Returns:      None
;
; Preserve:     All registers
;==============================================================================
NV4_PFIFO_DMA					EQU	00002508H
NV4_PFIFO_CACHE1_PUSH1                          EQU   	00003204H
NV4_PFIFO_CACHE1_DMA_PUSH                       EQU   	00003220H
NV4_PFIFO_CACHE1_DMA_PUSH_BUFFER_EMPTY          EQU   	00000100H
NV_PFIFO_CACHE1_PUSH1_MODE_DMA                  EQU   	00000100H
FIFO_CACHE1_STATUS				EQU	00003214H
CACHE1_STATUS_LOW_MARK_EMPTY			EQU	10H
GRAPHICS_STATUS					EQU	00400700H

EXTERNDEF       wFlatDataSel:WORD

DECPROC WaitUntilOtherContextsIdle, PASCAL, FRAME, NEAR
PARMD   pHWBoardCXOffset
OPENPROC
        PUSHR   eax,esi,edi,ds
        mov     ds,cs:[wFlatDataSel]
        mov     esi,pHWBoardCXOffset
        mov     edi,ds:[esi].HWBOARDCX.pChipRegs

busyp:  mov     eax,ds:[edi + NV4_PFIFO_DMA]
        and     eax,ds:[esi].HWBOARDCX.dwGdiPushBufferMask
        jnz     busyp

        mov     eax,ds:[edi + NV4_PFIFO_CACHE1_PUSH1]
        test    eax,NV_PFIFO_CACHE1_PUSH1_MODE_DMA
        jz      syncd
        and     eax,1FH
        cmp     eax,ds:[esi].HWBOARDCX.dwGdiPushBufferIndex
        je      syncd

emptyp: mov     eax,ds:[edi + NV4_PFIFO_CACHE1_DMA_PUSH]
        and     eax,NV4_PFIFO_CACHE1_DMA_PUSH_BUFFER_EMPTY
        jz      emptyp

syncd:
	;
	mov	eax,ds:[edi + FIFO_CACHE1_STATUS]
	and	eax,CACHE1_STATUS_LOW_MARK_EMPTY
	je	syncd
@@:
	; sync on graphics engine not idle
	mov	eax,ds:[edi + GRAPHICS_STATUS]
	and	eax,1
	jne	@B

        POPR    eax,esi,edi,ds
CLOSEPROC


;==============================================================================
;
; Function:     FlushChipsetCache
;
; Purpose:      This function does some things to help motherboard
;               chipsets get their cache coherency correct.
;
; Arguments:    None
;
; Returns:      None
;
; Preserve:     All registers
;==============================================================================
DECPROC FlushChipsetCache, PASCAL, NOFRAME, NEAR
OPENPROC
        PUSHR   eax,edx
        mov     edx,3D0H
        xor     eax,eax
        out     dx,al
        POPR    eax,edx
CLOSEPROC


;==============================================================================
;
; Function:     WaitForDmaBufferSpace
;
; Purpose:      This function is called when there are not enough
;               bytes left in the DMA buffer to place a command.
;               Note that this does not necessarily mean that you
;               have reached the end of the DMA buffer, although
;               that is one possibility. It might mean that if you
;               write more commands, you will write over data that
;               the DMA has not yet fetched. In either case, though,
;               waiting of some type is required, and this routine
;               can handle it.
;
; Arguments:
;               HWBOARDCX.pCurrentPushBufferOffset must be up to date.
;
; Returns:      The PDEVICE.HWPDevice.dwCurrentPushBufferOffset
;               field has been modified to the new place to put
;               DMA commands. Also the PDEVICE.HWPDevice.dwBytesFree
;               member has been modified to reflect the new amount
;               of space. You are guaranteed that the new DMA
;               offset can hold at least the amount of bytes you
;               requested as the argument to this routine.
;
; Preserve:     All registers should be preserved
;==============================================================================
DECPROC WaitForDmaBufferSpace, PASCAL, FRAME, NEAR
PARMD   pHWBoardCXOffset
PARMD   dwBytesNeeded
OPENPROC
        PUSHR   ds,alld
        mov     ds,cs:[wFlatDataSel]
        mov     esi,pHWBoardCXOffset
        mov     edi,ds:[esi].HWBOARDCX.pCurrentPushBufferOffset
        SETPUTPTR ds,esi,edi

IFDEF   DEBUG
        mov     ds:[esi].HWBOARDCX.dwPushBufferOffsetOnWaitEnter,edi
        mov     eax,dwBytesNeeded
        mov     ds:[esi].HWBOARDCX.dwLastByteRequest,eax
ENDIF

        ; First, regardless of where the GET ptr is, is there enough
        ; space between the PUT ptr and the end of the DMA buffer?
        mov     ecx,dwBytesNeeded
        add     ecx,04H
        mov     ebx,ds:[esi].HWBOARDCX.pPushBufferStartOffset
        add     ebx,ds:[esi].HWBOARDCX.dwPushBufferLength
        sub     ebx,edi
        cmp     ebx,ecx
        jl      WrapPut

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; If we get here, the DMA push buffer looks as follows:
        ;
        ;                          >= dwBytesNeeded + 4
        ;                |--------------------------------------------|
        ;
        ;               PUT
        ;                |
        ;                v
        ; |-----------------------------------------------------------|
        ;
        ; In this case, we need to read the GET ptr and see where it is.
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

WaitWhileGetGTPut:
        mov     eax,ds:[esi].HWBOARDCX.dwDmaGetPtr
        add     eax,ds:[esi].HWBOARDCX.pPushBufferContextStartOffset
        mov     ds:[esi].HWBOARDCX.dwLastGet,eax
        cmp     eax,edi
        jbe     GetLEPut

GetGTPut:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; GET is greater than PUT as shown in the diagram below. We
        ; might need to wait a bit until GET is enough greater than
        ; PUT to allow for the bytes needed plus maybe a JMP command.
        ;
        ;                          >= dwBytesNeeded + 4
        ;                |--------------------------------------------|
        ;
        ;                          ??????
        ;                |-----------------------|
        ;
        ;               PUT                     GET
        ;                |                       |
        ;                v                       v
        ; |-----------------------------------------------------------|
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        ; Wait until GET - PUT is at least dwBytesNeeded + 4
        sub     eax,edi
        sub     eax,ecx
        jc      WaitWhileGetGTPut

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; GET is enough greater than PUT as shown in the diagram below
        ; that we can grant the space needed.
        ;
        ;                  >= dwBytesNeeded + 4
        ;                |-----------------------|
        ;
        ;               PUT                     GET
        ;                |                       |
        ;                v                       v
        ; |-----------------------------------------------------------|
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        mov     ds:[esi].HWBOARDCX.dwBytesFree,eax

IFDEF DEBUG
        mov     ds:[esi].HWBOARDCX.dwLastWaitPath,1
        mov     ds:[esi].HWBOARDCX.dwLastBytesFree,eax
        PUSHR   eax,edi
        mov     eax,ds:[esi].HWBOARDCX.dwBytesFree
        add     edi,ecx
        sub     edi,4
        add     eax,edi
        mov     ds:[esi].HWBOARDCX.dwTempEnd,eax
        POPR    eax,edi
ENDIF
        jmp     Done

GetLEPut:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; The GET is less than the PUT which gives the sceanrio shown
        ; below. We already know that there is enough room from PUT
        ; to the end of the DMA buffer, so grant the request.
        ;
        ;                               >= dwBytesNeeded + 4
        ;                        |------------------------------------|
        ;
        ;          GET          PUT
        ;           |            |
        ;           v            v
        ; |-----------------------------------------------------------|
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        mov     eax,ds:[esi].HWBOARDCX.pPushBufferStartOffset
        add     eax,ds:[esi].HWBOARDCX.dwPushBufferLength
        sub     eax,edi
        sub     eax,ecx
        mov     ds:[esi].HWBOARDCX.dwBytesFree,eax

IFDEF DEBUG
        mov     ds:[esi].HWBOARDCX.dwLastWaitPath,2
        mov     ds:[esi].HWBOARDCX.dwLastBytesFree,eax
        PUSHR   eax,edi
        mov     eax,ds:[esi].HWBOARDCX.dwBytesFree
        add     edi,ecx
        sub     edi,4
        add     eax,edi
        mov     ds:[esi].HWBOARDCX.dwTempEnd,eax
        POPR    eax,edi
ENDIF
        jmp     Done

WrapPut:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; If we get here, there are not enough bytes from PUT to the end
        ; of the DMA buffer to hold the number of bytes needed as the
        ; diagram below shows. In this case, we must wait until the GET
        ; ptr goes less than or equal to the PUT ptr.
        ;
        ;                                    < dwBytesRequested + 4
        ;                                |----------------------------|
        ;
        ;                               PUT
        ;                                |
        ;                                v
        ; |-----------------------------------------------------------|
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Loopy:  mov     eax,ds:[esi].HWBOARDCX.dwDmaGetPtr
        add     eax,ds:[esi].HWBOARDCX.pPushBufferContextStartOffset
        mov     ds:[esi].HWBOARDCX.dwLastGet,eax
        cmp     eax,edi
        ja      WaitWithBusFree
        lea     edx,[esi].HWBOARDCX.bDmaPushBuffer
        cmp     eax,edx
        ja      GetLEPutWrapPut

WaitWithBusFree:
        mov     edx,100H
@@:     dec     edx
        jne     @B
        jmp     Loopy


GetLEPutWrapPut:
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; Having waited, the situation appears as shown. Now we can place
        ; a JMP command in the DMA buffer at the current PUT ptr to jmp
        ; back to the start of the buffer.
        ;
        ;                            < dwBytesRequested + 4
        ;                        |------------------------------------|
        ;
        ;         GET           PUT
        ;          |             |
        ;  X       v             v
        ; |-----------------------------------------------------------|
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        mov     ebx,edi
        lea     edi,[esi].HWBOARDCX.bDmaPushBuffer
        mov	eax,edi
        sub     eax,ds:[esi].HWBOARDCX.pPushBufferContextStartOffset
        add     eax,JMP_COMMAND
        mov     dword ptr ds:[ebx],eax
        SETPUTPTR ds,esi,edi

        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        ; After the JMP_COMMAND takes effect (it might be a little bit),
        ; eventually the scenario will be as shown. At this point, all
        ; we need to do is wait until either GET = PUT (which may happen
        ; if the GET reaches the JMP_COMMAND and executes it), OR until
        ; GET is at least dwBytesNeeded from the start of the DMA buffer.
        ;
        ;    dwBytesRequested + 4
        ; |-------------------------|
        ;
        ; PUT           GET
        ;  |             |
        ;  v             v
        ; |-----------------------------------------------------------|
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
@@:
        mov     eax,ds:[esi].HWBOARDCX.dwDmaGetPtr
        add     eax,ds:[esi].HWBOARDCX.pPushBufferContextStartOffset
        mov     ds:[esi].HWBOARDCX.dwLastGet,eax
        sub     eax,edi
        je      DmaIsIdle
        sub     eax,ecx
        jc      @B

        ; Now there are two things that could happen.
        ; 1) GET will become equal to PUT -- this will happen if the
        ;    GET command reaches the jmp and and takes the jmp. It
        ;    will then stop because it has reached PUT (which is
        ;    set to the beginning of the buffer).
        ; 2) GET - OFFSET dwDmaPushBufferStart > dwBytesRequested + 4
        ;    This will happen if GET is still fetching data, but
        ;    it has now fetched enough from the beginning of the
        ;    buffer that there is room to put the requested command in.

        ; The DMA is not idle, but there is enough room to place
        ; dwBytesNeeded between the buffer start and the GET ptr.
        mov     ds:[esi].HWBOARDCX.dwBytesFree,eax

IFDEF DEBUG
        mov     ds:[esi].HWBOARDCX.dwLastWaitPath,3
        mov     ds:[esi].HWBOARDCX.dwLastBytesFree,eax
        PUSHR   eax,edi
        mov     eax,ds:[esi].HWBOARDCX.dwBytesFree
        add     edi,ecx
        sub     edi,4
        add     eax,edi
        mov     ds:[esi].HWBOARDCX.dwTempEnd,eax
        POPR    eax,edi
ENDIF
        jmp     Done

DmaIsIdle:
        mov     eax,ds:[esi].HWBOARDCX.dwPushBufferLength
        sub     eax,ecx
        mov     ds:[esi].HWBOARDCX.dwBytesFree,eax

IFDEF DEBUG
        mov     ds:[esi].HWBOARDCX.dwLastWaitPath,4
        mov     ds:[esi].HWBOARDCX.dwLastBytesFree,eax
        PUSHR   eax,edi
        mov     eax,ds:[esi].HWBOARDCX.dwBytesFree
        add     edi,ecx
        sub     edi,4
        add     eax,edi
        mov     ds:[esi].HWBOARDCX.dwTempEnd,eax
        POPR    eax,edi
ENDIF

Done:
        POPR    ds,alld
CLOSEPROC


IF  (PLACE_DST_BITMAP_IN_SCREEN_CORNER NE 0)
;==============================================================================
;
; Function:     AlignOffscreenBitmap
;
; Purpose:      This function can be used for debugging. It is usually
;               used by the SetDstBaseAndPitch functions to center the
;               current bitmap in the uppoer left had corner of the
;               screen. The CRT start address and pitch registers are
;               adjusted to make the offscreen bitmap visible and
;               readable.
;
; Arguments:    edx     Base address
;               ecx     pitch
;		esi	HWBOARDCX ptr
;
; Returns:      None.
;
; Preserve:     Everything
;==============================================================================
EXTERNDEF	wFlatDataSel:WORD

PUBLIC  AlignOffscreenBitmap
AlignOffscreenBitmap    PROC    NEAR

        PUSHR   alld,ds
        mov	ds,cs:[wFlatDataSel]

        mov	edi,ds:[esi].HWBOARDCX.pChipRegs
	shr	ecx,3

	; Set the base address
	mov	ds:[edi + 600800H],edx

        ; Unlock the registers
        mov     dx,3D4H
        mov     al,1FH
        out     dx,al
        inc     dx
        in      al,dx
        mov     ah,al
        mov     al,57H
        out     dx,al

        ; The CRT pitch registers are from low to high 13H and
        ; the top 3 bits of 19H.
        mov     dx,3D4H
        mov     al,13H
        out     dx,al
        inc     dx
        mov     al,cl
        out     dx,al

        dec     dx
        mov     al,19H
        out     dx,al
        inc     dx
        in      al,dx
        and     al,1FH
        mov	ah,ch
        and     ah,7H
        shl     ah,5
        or      al,ah
        out     dx,al

        dec     dx
        mov     al,25H
        out     dx,al
        inc     dx
        in      al,dx
        and     al,0DFH
        mov	ah,ch
        and	ah,08H
        shl     ah,5 - 3
        or      al,ah
        out     dx,al

        POPR    alld,ds
        ret

AlignOffscreenBitmap    ENDP
ENDIF

CLOSESEG _NVTEXT32

END

