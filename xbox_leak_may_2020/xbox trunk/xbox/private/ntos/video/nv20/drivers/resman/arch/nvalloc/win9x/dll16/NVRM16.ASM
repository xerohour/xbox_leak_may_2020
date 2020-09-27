; 
;  (C) Copyright NVIDIA Corporation Inc., 1995,1996. All rights reserved.
; 
.386

_DATA           SEGMENT WORD PUBLIC USE16 'DATA'

EXTERN  _ResManAPI : DWORD

_DATA           ENDS
        
_TEXT           SEGMENT WORD PUBLIC USE16 'CODE'

        ASSUME  ds:_DATA
;
; Call the resource manager.
; It is called as:
;
;       VOID FAR PASCAL CallRM((LPNVPARAMS)lpParams);
;
lpParams        EQU     [bp + 6]
RegEAX          EQU     0
RegEBX          EQU     4
RegECX          EQU     8
RegEDX          EQU     12
RegEDI          EQU     16
RegESI          EQU     20
RegES           EQU     24
RegDS           EQU     28
RegCS           EQU     32

public  CALLRM
CALLRM          PROC NEAR
        push    bp
        mov     bp, sp
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        push    esi
        push    es
        les     si,  DWORD PTR lpParams
        mov     eax, DWORD PTR es:[si + RegEAX]
        push    eax
        mov     ebx, DWORD PTR es:[si + RegEBX]
        mov     ecx, DWORD PTR es:[si + RegECX]
        mov     edx, DWORD PTR es:[si + RegEDX]
        mov     edi, DWORD PTR es:[si + RegEDI]
        mov     ax,   WORD PTR es:[si + RegES]
        mov     esi, DWORD PTR es:[si + RegESI]
        mov     es, ax
        pop     eax
        call    DWORD PTR _ResManAPI
        push    eax
        push    esi
        mov     ax, es
        les     si, DWORD PTR lpParams
        mov     WORD PTR es:[si + RegES], ax
        pop     eax
        mov     DWORD PTR es:[si + RegESI], eax
        pop     eax
        mov     DWORD PTR es:[si + RegEAX], eax
        mov     DWORD PTR es:[si + RegEBX], ebx
        mov     DWORD PTR es:[si + RegECX], ecx
        mov     DWORD PTR es:[si + RegEDX], edx
        mov     DWORD PTR es:[si + RegEDI], edi
        pop     es
        pop     esi
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        leave
        retf	4
CALLRM          ENDP
;
; Read and write 32 bit registers.
; Called as:
;
;       VOID  FAR PASCAL REG_WR32(FAR DWORD *Address, DWORD Data);
;       DWORD FAR PASCAL REG_RD32(FAR DWORD *Address);
;
RdAddress       EQU     [bp + 6]
WrAddress       EQU     [bp + 10]
WrData          EQU     [bp + 6]

public  REG_WR32
REG_WR32        PROC NEAR
        push    bp
        mov     bp, sp
        push    eax
        push    si
        push    es
        les     si,  DWORD PTR WrAddress
        mov     eax, WrData
        mov     DWORD PTR es:[si], eax
        pop     es
        pop     si
        pop     eax
        leave
        retf	8
REG_WR32        ENDP
public  REG_RD32
REG_RD32        PROC NEAR
        push    bp
        mov     bp, sp
        push    ecx
        push    si
        push    es
        les     si,  DWORD PTR RdAddress
        mov     ecx, DWORD PTR es:[si]
        mov     ax, cx
        shr     ecx, 16
        mov     dx, cx
        pop     es
        pop     si
        pop     ecx
        leave
        retf	4
REG_RD32        ENDP
;
; Read and write 32 bit registers.
; Called as:
;
;       VOID  FAR PASCAL REG_WR32(FAR DWORD *Address, DWORD Data);
;       DWORD FAR PASCAL REG_RD32(FAR DWORD *Address);
;
RdAddress       EQU     [bp + 6]
WrAddress       EQU     [bp + 10]
WrData          EQU     [bp + 6]

public  REG_WR08
REG_WR08        PROC NEAR
        push    bp
        mov     bp, sp
        push    eax
        push    si
        push    es
        les     si,  DWORD PTR WrAddress
        mov     al, WrData
        mov     BYTE PTR es:[si], al
        pop     es
        pop     si
        pop     eax
        leave
        retf	8
REG_WR08        ENDP
public  REG_RD08
REG_RD08        PROC NEAR
        push    bp
        mov     bp, sp
        push    ecx
        push    si
        push    es
        les     si,  DWORD PTR RdAddress
        mov     al, BYTE PTR es:[si]
        pop     es
        pop     si
        pop     ecx
        leave
        retf	4
REG_RD08        ENDP
;
; Hide the emulated cursor in the framebuffer.
;
; Called as:
;
;        NvSysHideEmulatedCursor(WORD, WORD, WORD, WORD, WORD, WORD, WORD, WORD, DWORD FAR *);
;
xGlyphPos       EQU     [bp + 24]
yGlyphPos       EQU     [bp + 22]
cxGlyphWidth    EQU     [bp + 20]
cyGlyphHeight   EQU     [bp + 18]
cxScreenWidth   EQU     [bp + 16]
cyScreenHeight  EQU     [bp + 14]
cScreenDepth    EQU     [bp + 12]
selScreen       EQU     [bp + 10]
lpSaveUnder     EQU     [bp + 6]
public NvSysHideEmulatedCursor
NvSysHideEmulatedCursor PROC NEAR
        push    bp
        mov     bp, sp
        push    ds
        push    es
        push    esi
        push    edi
        push    edx
        push    ecx
        push    ebx
        mov     es, selScreen
        xor     esi, esi
        lds     si, lpSaveUnder
;
; Bound horizontal parameters to visible screen.
;
        mov     ax, xGlyphPos
        or      ax, ax
        jge     @f
        add     ax, cxGlyphWidth
        jl      HideCursorExit
        mov     cxGlyphWidth, ax
        xor     ax, ax
        mov     xGlyphPos, ax
@@:     mov     dx, cxScreenWidth
        cmp     ax, dx
        jge     HideCursorExit
        mov     cx, cxGlyphWidth
        or      cx, cx
        jz      HideCursorExit
        add     cx, ax
        cmp     cx, dx
        jl      @f
        sub     dx, ax
        mov     cxGlyphWidth, dx
@@: 
;
; Bound vertical parameters to visible screen.
;
        mov     ax, yGlyphPos
        or      ax, ax
        jge     @f
        add     ax, cyGlyphHeight
        jl      HideCursorExit
        mov     cyGlyphHeight, ax
        xor     ax, ax
        mov     yGlyphPos, ax
@@:     mov     dx, cyScreenHeight
        cmp     ax, dx
        jge     HideCursorExit
        mov     cx, cyGlyphHeight
        or      cx, cx
        jz      HideCursorExit
        add     cx, ax
        cmp     cx, dx
        jl      @f
        sub     dx, ax
        mov     cyGlyphHeight, dx
@@:
;
; Convert pixel depth into shift count.
;
        mov     cx, cScreenDepth
        shr     cx, 4
;
; Calc starting pixel position.
;
        xor     edi, edi
        xor     edx, edx
        mov     di, yGlyphPos
        mov     dx, cxScreenWidth
        imul    edi, edx
        xor     ebx, ebx
        mov     bx, xGlyphPos
        add     edi, ebx
        mov     bx, dx
        mov     dx, cxGlyphWidth
        add     edx, edi
        shl     edi, cl
        shl     edx, cl
        sub     edx, edi
        shl     ebx, cl
        mov     ax, di
        and     di, 0FFFCh
        and     ax, 03h
        add     ax, dx
        add     ax, 03h
        xor     edx, edx
        mov     dx, ax
        shr     ax, 2
        and     dl, 0FCh
        mov     cxGlyphWidth, ax
        sub     ebx, edx
        neg     edx
        add     edx, 128
;
; Loop through rows of pixel data.
;
        cld
        xor     ecx, ecx
RestoreRow:
        mov     cx, cxGlyphWidth
    rep movsd   es:[edi], [esi]
        add     edi, ebx
        add     esi, edx
        dec     WORD PTR cyGlyphHeight
        jnz     RestoreRow
;
; All done.
;        
HideCursorExit:        
        pop     ebx
        pop     ecx
        pop     edx
        pop     edi
        pop     esi
        pop     es
        pop     ds
        leave
        retf	20
NvSysHideEmulatedCursor ENDP

_TEXT           ENDS
;                END     LIBENTRY
                END
