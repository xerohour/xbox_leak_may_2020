;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         gndibxit.asm
;
; Purpose:      This file streamlines the exit process on SetDIBitsToDevice
;               and StretchDIBits.
;
;               The code is kind of messy -- I don't understand what
;               is going on everywhere, but I just tried to optimize.
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

EXPORT_ORDINAL_FOR_WRITE_PRINTER        TEXTEQU <269>
EXPORT_ORDINAL_FOR_GETDIBITS            TEXTEQU <441>
EXPORT_ORDINAL_FOR_POLYGON              TEXTEQU <36>
EXPORT_ORDINAL_FOR_ALLOCCSTODSALIAS     TEXTEQU <170>

;  From MSVC60/INCLUDE/OLENLS.H: 
;
;  A language ID is a 16 bit value which is the combination of a
;  primary language ID and a secondary language ID.  The bits are
;  allocated as follows:
;
;       +-----------------------+-------------------------+
;       |      Sublanguage ID   |   Primary Language ID   |
;       +-----------------------+-------------------------+
;        15                   10 9                       0   bit
;
;
;  Language ID creation/extraction macros:
;
;    MAKELANGID    - construct language id from primary language id and
;                    sublanguage id.
;    PRIMARYLANGID - extract primary language id from a language id.
;    SUBLANGID     - extract sublanguage id from a language id.
;
PRIMARY_LANG_MASK                       TEXTEQU <03FFh>

OPENSEG  _DATA, USE16, DATA
szGdi           DB      "GDI",0
szKernel        DB      "KERNEL",0
CLOSESEG _DATA

OPENSEG  _NVTEXT32, %DEF_CODE_SIZE, CODE

PUBLIC  bUseStretchDIBitsFastExit, bUseSetDIBitsFastExit
bUseSetDIBitsFastExit           DB      0
bUseStretchDIBitsFastExit       DB      0

PUBLIC  lpfnGdiCode03_20B7
lpfnGdiCode03_20B7      DD      0

PUBLIC  lpfnGdiCode03_2119
lpfnGdiCode03_2119      DD      0

CLOSESEG _NVTEXT32

OPENSEG  _NVTEXT16, USE16, CODE

PUBLIC  lpfnGdiCode43_165E
lpfnGdiCode43_165E      DD      0

PUBLIC  lpfnGdiCode03_20C9
lpfnGdiCode03_20C9      DD      0

PUBLIC  lpfnGdiCode25_01B8
lpfnGdiCode25_01B8      DD      0

PUBLIC  dwFlatKernelDataSeg
dwFlatKernelDataSeg     DD      0

PUBLIC  wKernelDataSeg, wLDTSelector
wKernelDataSeg          DW      0
wLDTSelector            DW      0

PUBLIC  dwTableRightToLeftLangIDs
dwTableRightToLeftLangIDs       LABEL   WORD
;       Arabic
DD      0001H
;       Hebrew.
DD      000DH
;       -1 signals the end of the table
DD      -1

szNlsLocalePath DB      "System\CurrentControlSet\Control\Nls\Locale",0

;==========================================================================
;
; Function:     GN_InitDibFastExit
;
; Purpose:      This routine inits stuff for the fast exit code on
;               SetDIBitsToDevice and StretchDIBits.
;
; Arguments:
;
; Returns:      None
;
; Preserve:     ds,esi,edi
;==========================================================================
DECPROC GN_InitDibFastExit, PASCAL, FRAME, FAR16
PARMD	lpNvmodeDispData
LOCALD  hLocaleKey
LOCALD  dwSize
LOCALS  szDefault,4
LOCALS  szDefaultLang,16
OPENPROC
        PUSHR   ds,esi,edi

        ; Clear these out to begin with
        mov     ds,cs:[wNVTEXT32_Alias_In_NVTEXT16]
        mov     ds:[bUseSetDIBitsFastExit],0
        mov     ds:[bUseStretchDIBitsFastExit],0

        mov     ds,cs:[wNVTEXT16_Alias_In_NVTEXT16]

        ; We are going to read the registry -- if we are on Arabic
        ; or Hebrew Windows (right to left reading languages), then
        ; we cannot do this patch.
        pushd   HKEY_LOCAL_MACHINE      ; hKey
        mov     ax,cs
        lea     ebx,szNlsLocalePath
        push    ax
        push    bx                      ; lpSubKey
        lea     ebx,hLocaleKey
        mov     ax,ss
        push    ax
        push    bx                      ; phkResult
        call    RegCreateKey
        shl     edx,10H
        mov     dx,ax
        cmp     edx,ERROR_SUCCESS
        jne     Done

        ; Read the "" string in this key. It should be equal
        ; to something like: "00000401" or "0000040D"  -- it
        ; will be the string version of a language identifier.
        sub     ecx,ecx
        mov     szDefault[0],cl
        mov     szDefaultLang[0],cl
        mov     dwSize,16
        mov     ax,ss
        shl     eax,10H
        push    hLocaleKey
        lea     ax,szDefault
        push    eax
        push    ecx
        push    ecx
        lea     ax,szDefaultLang
        push    eax
        lea     ax,dwSize
        push    eax
        call    RegQueryValueEx
        shl     edx,10H
        mov     dx,ax
        mov     edi,edx

        ; close the key
        push    hLocaleKey
        call    RegCloseKey

        ; If there was an error reading, then to be safe, leave
        ; the fast dib stuff off.
        cmp     edi,ERROR_SUCCESS
        jne     Done

        ; Convert the language ID returned to a number
        sub     edx,edx
        sub     ebx,ebx
        sub     eax,eax
@@:
	add	ebp,ebx
        movzx   ecx,byte ptr szDefaultLang
	sub	ebp,ebx
        cmp     ecx,'0'
        jc      NoMoreDigits
        cmp     ecx,'9'
        jbe     FoundNumber
        cmp     ecx,'A'
        jc      NoMoreDigits
        cmp     ecx,'F'
        jbe     FoundHexCap
        cmp     ecx,'a'
        jc      NoMoreDigits
        cmp     ecx,'f'
        ja      NoMoreDigits
FoundHexSmall:
        sub     ecx, 'a' - 10
        jmp     NextDigit
FoundHexCap:
        sub     ecx, 'A' - 10
        jmp     NextDigit
FoundNumber:
        sub     ecx, '0'
NextDigit:
        imul    eax,10H
        add     eax,ecx
        inc     ebx
        jmp     @B

NoMoreDigits:
        and     eax, PRIMARY_LANG_MASK

        ; Compare the primary language ID returned against all the
        ; right to left language IDs. If it matches any one of them,
        ; then no fast dib exit.
        mov     edi, OFFSET dwTableRightToLeftLangIDs
@@:
        mov     ebx, cs:[edi]
        add     edi, 4
        cmp     ebx, -1
        je      NoMatch
        cmp     eax, ebx
        jne     @B
        jmp     Done
NoMatch:

	push	lpNvmodeDispData
        CALLFAR16IND lpfnGetControlVisionFlag,es,ax
	cmp	ax,0FEH
	jne	Done

        ; Get GDI module handle
        mov     ax,_DATA
        mov     cx,OFFSET szGdi
        push    ax
        push    cx
        call    GetModuleHandle
        mov     si,ax

        ; I need the selector value of GDI code segment 43 and the
        ; WRITE_PRINTER function happens to be in Gdi Code segment 43.
        push    si
        pushd   EXPORT_ORDINAL_FOR_WRITE_PRINTER
        call    GetProcAddress
        shl     edx,10H
        or      edx,edx
        je      Done
        mov     dx,165EH
        mov     ds:[lpfnGdiCode43_165E],edx

        ; I need the selector value of GDI code segment 3 and the
        ; GetDIBits function happens to be in Gdi Code segment 3.
        push    si
        pushd   EXPORT_ORDINAL_FOR_GETDIBITS
        call    GetProcAddress
        shl     edx,10H
        or      edx,edx
        je      Done
        mov     dx,20C9H
        mov     ds:[lpfnGdiCode03_20C9],edx

        mov     ds,cs:[wNVTEXT32_Alias_In_NVTEXT16]
        mov     dx,2119H
        mov     ds:[lpfnGdiCode03_2119],edx
        mov     dx,20B7H
        mov     ds:[lpfnGdiCode03_20B7],edx

        mov     ds,cs:[wNVTEXT16_Alias_In_NVTEXT16]

        ; I need the selector value of GDI code segment 25 and the
        ; Polygon function happens to be in Gdi Code segment 25.
        push    si
        pushd   EXPORT_ORDINAL_FOR_POLYGON
        call    GetProcAddress
        shl     edx,10H
        or      edx,edx
        je      Done
        mov     dx,01B8H
        mov     ds:[lpfnGdiCode25_01B8],edx

        ; Get the module handle for KERNEL
        mov     ax,_DATA
        mov     cx,OFFSET szKernel
        push    ax
        push    cx
        call    GetModuleHandle
        mov     si,ax

        ; I need the Kernel Data selector which is in the Kernel Code
        ; segment 1 and offset 2. AllocCSToDSAlias is in Kernel Code Seg 1.
        push    si
        pushd   EXPORT_ORDINAL_FOR_ALLOCCSTODSALIAS
        call    GetProcAddress
        or      dx,dx
        je      Done
        mov     di,dx

        ; Get the kernel data seg and the flat data seg pointer.
        ; The fast exit code requires that the Data seg selector be
        ; an LDT selector and not a GDT selector.
        mov     es,di
        sub     ebx,ebx
        movzx   eax,word ptr es:[ebx + 02H]
        test    eax,4
        jz      Done
        mov     ds:[wKernelDataSeg],ax
        push    ax
        call    GetSelectorBase
        shl     edx,10H
        mov     dx,ax
        mov     ds:[dwFlatKernelDataSeg],edx

        ; Get the selector which points to the base of the LDT.
        ; The fast exit code requires that this selector be
        ; an LDT selector and not a GDT selector.
        mov     es,di
        sub     ebx,ebx
        movzx   eax,word ptr es:[ebx + 06H]
        test    eax,4
        jz      Done
        mov     ds:[wLDTSelector],ax

        ; Everything worked! Set the flags saying we can do fast exit.
        mov     ds,cs:[wNVTEXT32_Alias_In_NVTEXT16]
        mov     ds:[bUseSetDIBitsFastExit],1
        mov     ds:[bUseStretchDIBitsFastExit],1

Done:
        POPR    ds,esi,edi
CLOSEPROC


;==========================================================================
;
; Function:     GdiCode03_2119
;               GdiCode03_217C
;
; Purpose:      This is the routine we return to from SetDibBitsToDevice.
;               It is in GDI Code segment 3 at offset 2119. We pick up
;               execution at this point.
;               StretchDIBits initially returns to Gdi Code segment 3
;               at offset 20B7. But almost always (and the only time we
;               come here), goes on to Gdi Code segment 3 at offset 217C.
;
; Arguments:    eax     Return Value from SetDIBitsToDevice
;               20H bytes of parameters
;               STACK:  CODE3:2119
;               OldEbp
;
; Returns:      None
;
; Preserve:     ds,es,fs
;==========================================================================
GdiCode03_2119	PROC    FAR16
        ; Don't know what this test is for, but do it
        test    byte ptr ss:[di + 02H],40H
        jz      G3_20C9
        .errnz  $ - OFFSET GdiCode03_217C
GdiCode03_2119	ENDP

GdiCode03_217C	PROC    FAR16
        ; We may need to free this handle
        cmp     word ptr ss:[bp - 28H],0
        jne     G3_2183

G3_2189:
        ; I don't know what this does.
        cmp     dword ptr ss:[bp - 32H],0
        jne     G3_2192

G3_2199:
        ; I don't know what this does.
        and     byte ptr ds:[si + 00FEH],0EFH

        ; I know this test has to do with ICM. If ICM was involved
        ; then we jump and clear out the ICM ptr. That's all I know.
        test    byte ptr ds:[si + 00B6H],80H
        jnz     G3_21A9

G3_21B1:
        ; Clear these off to go "out" to the next routine
        pop     edi
        pop     si
        pop     cx
        mov     ds,cx
        mov     sp,bp
        pop     bp

        ; Check the return address. It must be Code43:165E or
        ; we cannot handle it ourselves
        mov     ecx,cs:[lpfnGdiCode43_165E]
        movzx   ebx,sp
        cmp     ecx,ss:[ebx]
        je      G43_165E
        RETFAR16 20H

PLABEL  G3_20C9
        ; This path doesn't seem to happen much in the benchmark,
        ; so I will just jmp back in line to the correct place.
        mov     ss:[bp - 02H],ax
        jmp     FFP16 PTR cs:[lpfnGdiCode03_20C9]

G3_2183:
        ; Free the handle
        push    eax
        push    word ptr ss:[bp - 28H]
        call    GlobalFree
        pop     eax
        jmp     G3_2189

G3_2192:
        push    eax
        push    dword ptr ss:[bp - 32H]
        call    FFP16 PTR cs:[lpfnGdiCode25_01B8]
        pop     eax
        jmp     G3_2199

G3_21A9:
        mov     dword ptr ds:[si + 006EH],0
        jmp     G3_21B1
GdiCode03_217C	ENDP
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;==========================================================================
;
; Function:     G43_165E
;
; Purpose:      This is the routine we evetually end up at when we return
;               from SetDibBitsToDevice or StretchDIBits.
;               It is in GDI Code segment 43 at offset 165E -- G43_165E.
;
; Arguments:    eax     return value from SetDIBitsToDevice or
;                       StretchDIBits
;
; Returns:      None
;
; Preserve:     ds,es,fs
;==========================================================================
; These parameters are valid at this point.
dwSelectorColonFlag     TEXTEQU <dword ptr ss:[bp - 04H]>
wSelector1ToFree        TEXTEQU < word ptr ss:[bp - 02H]>
wSelector2ToFree        TEXTEQU < word ptr ss:[bp - 0AH]>

PUBLIC  G43_165E
G43_165E:
        ; This mimicks a RETFAR16 20H
        add     sp,24H

K2_7C9D:
        ; Save this guy!
        push    eax

        ; If both the selector and flag are zero, then nothing to free
        mov     eax,dwSelectorColonFlag
        or      ax,ax
        je      G43_166B
        test    eax,0FFFF0000H
        jz      G43_166B

        ; We will be needing the following register values throughout
        ; the routines so load them up now.
        mov     ds,cs:[wKernelDataSeg]
        mov     es,cs:[wLDTSelector]
        mov     fs,ds:[13A6H]

        ; Call the kernel routine to clean up one of the selectors
        ; it allocated to map a flat ptr for the thunk.
        movzx   edx,wSelector1ToFree
        call    K1_4BA0

G43_166B:
K1_593B:
        movzx   edx,wSelector2ToFree
        sub     edi,edi
        and     edx,0FFFFFFF8H
        je      K1_59BD

        movzx   eax,word ptr ds:[edi + 1A50H]
        sub     ecx,ecx
        mov     es:[edx + 00H],eax
        mov     es:[edx + 04H],ecx
        mov     ds:[edi + 1A50H],dx
        inc     word ptr ds:[edi + 1A52H]

K1_5970:
        cmp     word ptr ds:[edi + 1A52H],2AH
        jc      K1_59BD
        dec     word ptr ds:[edi + 1A52H]
        mov     ds:[edi + 1A50H],ax
        mov     dword ptr es:[edx + 00H],1
        mov     dword ptr es:[edx + 04H],0F300H
        or      edx,7
        call    K1_4BA0
        sub     edi,edi
        jmp     K1_5970

K1_59BD:
G43_168F:
        ; Restore the return value
        pop     eax
        mov     ebx,34H
        mov     sp,bp
        RETFAR32

; This is the end of the routine G43_165E
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;==============================================================================
;
; Function:     K1_4BA0
;
; Purpose:      This function is my implementation of K1_4BA0 -- Kernel
;               code segment 1, the routine at offset 4BA0. K1_4BA0
;               actually calls a number of other routines which call
;               other routines, etc. I have flattened the whole thing
;               and eliminated ALOT of code. I have tried to put
;               labels in my implementation that link to the correct
;               Kernel offset so that it is easier to see the
;               correspondence between the two.
;
; Arguments:    edx     selector to deal with
;
; Returns:      None
;
; Preserve:     ds,es,fs
;==============================================================================
PUBLIC  K1_4BA0
K1_4BA0:
K1_356A:
K1_356F:
        sub     edi,edi
        mov     gs,ds:[121EH]

        ; I don't know what this is for
        cmp     edi,ds:[12E0H]
        je      K1_358D

K1_7F14:
        ; I don't know what this does
        mov     ebx,1364H
        mov     ecx,ds:[12E4H]
        mov     ecx,fs:[ecx]
        inc     dword ptr fs:[ecx + 38H]
        dec     dword ptr ds:[ebx + 10H]
        jne     K1_7F37
        inc     dword ptr ds:[ebx + 04H]
        mov     ds:[ebx + 08H],ecx
        jmp     K1_358D
K1_7F37:
        cmp     ds:[ebx + 08H],ecx
        jne     K1_7F43
        inc     dword ptr ds:[ebx + 04H]

K1_358D:
        ; I don't know what this does
        inc     word ptr gs:[edi + 1EH]

K1_4BAB:
K1_555D:
        ; Updating special arena area to indicate selector is going away.
        ; This is NOT the LDT.
        mov     gs,ds:[13AAH]
        mov     eax,ds:[1234H]
        mov     ebx,edx
        and     ebx,0FFFFFFF8H
        shr     ebx,1
        mov     gs:[eax + ebx],edi

K1_4BC2:
K1_4C85:
        ; Assume just 1 selector in the tile
        mov     ecx,1

        ; Make sure the selector is valid
        lar     ax,dx
        jne     K1_4CBD

        ; Test for system segment
        test    ah,10H
        jz      K1_4CBD

        ; Test for segment present
        test    ah,80H
        jz      K1_4CA4

K1_4CB1:
        ; Segment is present. The number of tiled selectors is
        ; the number of 64K chunks in the selector limit. If lsl
        ; fails, then ecx is unchanged -- i.e. just 1 selector.
        lsl     ecx,edx
        jnz     K1_4CBD
        shr     ecx,10H
        inc     ecx

K1_4CBD:
K1_4BF0:
        cmp     edi,ds:[12E0H]
        je      K1_4C18

K1_4C10:
K1_7F00:
K1_7E8E:
K1_7F0A:
K1_7D85:
        mov     eax,ds:[12E4H]
        mov     ebx,1364H
        mov     eax,fs:[eax]
        inc     dword ptr fs:[eax + 38H]
        dec     dword ptr ds:[ebx + 10H]
        je      K1_7DCD
        cmp     eax,ds:[ebx + 08H]
        jne     K1_7DB5
        inc     dword ptr ds:[ebx + 04H]
        jmp     K1_7DE9
K1_7DB5:
        add     ebx,cs:[dwFlatKernelDataSeg]
        mov     ah,0AH
        call    FFP16 PTR ds:[1A80H]
        jmp     K1_7DE9
K1_7DCD:
        inc     dword ptr ds:[ebx + 04H]
        mov     ds:[ebx + 08H],eax

K1_7DE9:
K1_4C18:
        mov     ebx,edx
        and     ebx,00000FFF8H
        mov     esi,ebx
        shr     esi,1
        cmp     si,ds:[1232H]
        jnc     K1_4C52

K1_4C30:
        ; The unused descriptors in the LDT are a linked list!
        ; ds:[13ACH] points to the head and ds:[13AEH] is the
        ; number of unused descriptors (e.g. selectors). The
        ; low word of a descriptor entry gives the next selector
        ; that is unused.
        movzx   esi,word ptr ds:[13ACH]
        inc     word ptr ds:[13AEH]
        movzx   eax,word ptr es:[esi]
        mov     es:[esi],bx
        mov     es:[ebx],ax
        mov     word ptr es:[ebx + 02H],di
        mov     dword ptr es:[ebx + 04H],0F0000H

K1_4C60:
        ; Once again -- don't know what this stuff does
        cmp     edi,ds:[12E0H]
        je      K1_4C76

K1_4C73:
K1_7FD1:
K1_7FDC:
K1_7DF2:
        mov     ebx,1364H
        dec     dword ptr ds:[ebx + 04H]
        je      K1_7E08
        inc     dword ptr ds:[ebx + 10H]
        jmp     K1_7E2C
K1_7E08:
        mov     ds:[ebx + 08H],edi
        inc     dword ptr ds:[ebx + 10H]
        jle     K1_7E1A

K1_7E2C:
K1_7FDF:
K1_7EA9:
        mov     eax,ds:[12E4H]
        mov     eax,fs:[eax]
        dec     dword ptr fs:[eax + 38H]
        jne     K1_7EFF
        cmp     edi,fs:[eax + 4CH]
        je      K1_7EFF
        mov     ah,12H
        call    FFP16 PTR ds:[1A80H]
        mov     eax,ds:[12E0H]
        mov     eax,fs:[eax]
        btr     dword ptr fs:[eax + 44H],7
        jnc     K1_7EFF
        movzx   eax,sp
        mov     ax,ss:[eax + 0FFFFF000H]
K1_7EFF:
K1_7FE2:
K1_4C76:
K1_4CC1:
        add     edx,8
        dec     ecx
        jne     K1_4CBD

K1_4CCB:
K1_4BDE:
K1_4BE4:
K1_359D:
        mov     gs,ds:[121EH]
        dec     word ptr gs:[edi + 1EH]
        jne     K1_35B2
        test    word ptr gs:[edi + 44H],1
        jz      K1_35B2
        and     word ptr gs:[edi + 44H],-2
        int     2
K1_35B2:
        cmp     edi,ds:[edi + 12E0H]
        je      K1_35D2

K1_35C9:
K1_7F56:
        mov     ebx,1364H
        dec     dword ptr ds:[ebx + 04H]
        jne     K1_7FA4
        mov     ds:[ebx + 08H],edi
        inc     dword ptr ds:[ebx + 10H]
        jle     K1_7F8E
K1_7F6C:
        mov     eax,ds:[edi + 12E4H]
        mov     eax,fs:[eax]
        dec     dword ptr fs:[eax + 38H]
        jne     K1_7F8B
        cmp     edi,fs:[eax + 4CH]
        jne     K1_7FAA
        jmp     K1_7F8B

K1_7FA4:
        inc     dword ptr ds:[ebx + 10H]
        jmp     K1_7F8B
K1_7FAA:
        mov     ah,12H
        call    FFP16 PTR ds:[1A80H]
        mov     eax,ds:[edi + 12E0H]
        mov     eax,fs:[eax]
        btr     dword ptr fs:[eax + 44H],7
        jnc     K1_7F8B
        movzx   eax,sp
        mov     ax,ss:[eax + 0FFFFF000H]
        jmp     K1_7F8B

K1_7F8B:
K1_35D2:
K1_4BED:
        ret

; This is the end of routine K1_4BA0. We need it a few times, so
; we made it a sub-routine.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

K1_7F43:
K1_5830:
        ; Make a flat ptr to wKernelDataSeg+1364H and
        ; Service to main VDD -- I don't know what it does
        add     ebx,cs:[dwFlatKernelDataSeg]
        mov     ah,0AH
        call    FFP16 PTR ds:[1A80H]
        jmp     K1_358D

K1_4CA4:
        ; If the segment is not present then Windows stores the
        ; number of contiguous tiled selectors in the top byte
        ; of the LDT entry for the selector.
        movzx   ecx,byte ptr es:[ebx + 07H]
        jmp     K1_4CBD

K1_7E1A:
        add     ebx,cs:[dwFlatKernelDataSeg]
        mov     ah,0BH
        call    FFP16 PTR ds:[1A80H]
        jmp     K1_7E2C

K1_7F8E:
        add     ebx,cs:[dwFlatKernelDataSeg]
        mov     ah,0BH
        call    FFP16 PTR ds:[1A80H]
        jmp     K1_7F6C

K1_4C52:
        or      ebx,7
        mov     eax,1
        int     31H
        jmp     K1_4C60

CLOSESEG  _NVTEXT16

END

