;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gnline.asm
;
; Purpose:      This file holds the Output(OS_POLYLINE) display driver
;               implementation. The Output display driver entry point
;               jumps to here without creating a FRAME when it is
;               discovered that this is and OS_POLYLINE style.
;
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

OPENSEG  _DATA, USE16, DATA
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

;==============================================================================
;
; Function:     GENERIC_Polyline
;
; Purpose:      This function is the Output(OS_POLYLINE) display driver
;               implementation. The Output display driver entry point
;               jumps to here without creating a FRAME when it is
;               discovered that this is the OS_POLYLINE style.
;
; Arguments:    Output Display Driver Stack Frame
;
; Returns:      ax      0 failure
;                       1 success
;
; Preserve:     ds,esi,edi
;==============================================================================
DECPROC GENERIC_Polyline, PASCAL, FRAME, FAR16
PARMD   lpDstDev
PARMW   wStyle
PARMW   wCount
PARMD   lpPoints
PARMD   lpPen
PARMD   lpBrush
PARMD   lpDrawMode
PARMD   lpClipRect
OPENPROC
        PUSHR   ds,esi,edi

        ; Load HW Logical Device Context
        mov     ds,cs:[wFlatDataSel]
        mov     ebx,cs:[CSpGNLogicalDevice]

        ; Get a ptr to the dst bitmap
        mov     gs,word ptr lpDstDev+2
        movzx   esi,word ptr lpDstDev+0

        ; Load dst flags and prep some registers
        sub     edx,edx
        sub     edi,edi
        movzx   eax,gs:[esi].DIBENGINE.deFlags

        ; Filter out Null Pens
        cmp     edx,lpPen
        je      GN_PolylinePunt

        ; Filter out old style destinations
        cmp     dx,gs:[esi].DIBENGINE.deType
        je      GN_PolylinePunt

        ; Filter out based upon deFlags
        test    ah,VRAM / 256
        jz      GN_PolylinePunt

        cmp     ds:[ebx].GNLOGDEVCX.dwHardwareBusy,0
        jne     GN_PolylinePunt

        ; Load up the pen color
        les     di,lpPen
        mov     ecx,es:[edi].DIB_Pen.dpPenColor
        movzx   edx,byte ptr es:[edi].DIB_Pen.dpPenStyle
        cmp     edx,LS_NOLINE
        je      GN_PolylineSuccess
        
        test    ah,PALETTE_XLAT / 256
        jz      @F

        ; Get color from bPaletteXlatTable
        and     ecx,0FFh
        lea     eax,ds:[ebx].GNLOGDEVCX.bPaletteXlatTable
        mov     cl,byte ptr ds:[eax+ecx]

@@:
        ; Push arguments for Hardware specific polyline routine
        push    ds:[ebx].GNLOGDEVCX.pHWLogdevCX
        push    lpPoints
        movzx   eax,wCount
        push    eax
        push    ecx

        ; We need the background color in case this is a styled line
        les     di,lpDrawMode
        mov     eax,es:[edi].DRAWMODE.bkColor
        push    eax

        ; Get the Rop2 from the DRAWMODE struct
        movzx   eax,byte ptr es:[edi].DRAWMODE.Rop2
        dec     eax
        push    eax
        push    edx

        ; Get the clip rect if there is one
        mov     eax,-1
        cmp     lpClipRect,0
        je      NoClipRect
        les     di,lpClipRect
        mov     eax,dword ptr es:[edi].RECT.left
        mov     ebx,dword ptr es:[edi].RECT.right

        ; Right may exceed bitmap Width
        cmp     bx,gs:[esi].DIBENGINE.deWidth
        jle     @F
        mov     bx,gs:[esi].DIBENGINE.deWidth
@@:

NoClipRect:
        push    ebx
        push    eax

        ; Check for degenerate clip rectangles before calling.
        ; This almost never happens, but should be checked for.
        cmp     ax,bx
        jge     GN_PolylinePuntPop
        shr     eax,10H
        shr     ebx,10H
        cmp     ax,bx
        jge     GN_PolylinePuntPop

        cmp     edx,LS_SOLID
        jne     GN_PolylineNonSolid

        ; Call the hardware specific routine to draw the lines
        call    HW_Polyline
        test    eax,eax
        jz      GN_PolylinePunt
        jmp     GN_PolylineSuccess
GENERIC_Polyline        ENDP


PUBLIC  GN_PolylineNonSolid
GN_PolylineNonSolid PROC    NEAR
        call    HW_PolyStyledLine
        test    eax,eax
        jz      GN_PolylinePunt
        jmp     GN_PolylineSuccess
GN_PolylineNonSolid ENDP

;==============================================================================
;
; Function:     Polyline ending routines
;
; Purpose:      These functions are all the possible ways of getting out
;               of Polyline. You can either return a success code,
;               punt to the DIBENG, etc.
;
; Arguments:    Output Display Driver Stack Frame
;
;==============================================================================
PUBLIC  GN_PolylinePuntPop
GN_PolylinePuntPop       PROC    NEAR
        ; Clean off parameters for HW_Polyline
        add     sp,9 * 4
GN_PolylinePuntPop       ENDP

PUBLIC  GN_PolylinePunt
GN_PolylinePunt  PROC    NEAR
        POPR    ds,esi,edi
        STOPPROC
PLABEL  Punt_Polyline
        jmp     DIB_Output
GN_PolylinePunt  ENDP

PUBLIC  GN_PolylineSuccess
GN_PolylineSuccess       PROC    NEAR
        mov     eax,1
        POPR    ds,esi,edi
        STOPPROC
        RETFAR16 1CH
GN_PolylineSuccess       ENDP

CLOSESEG _NVTEXT32

END

