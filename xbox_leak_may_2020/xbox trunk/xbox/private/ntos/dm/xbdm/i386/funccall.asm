        title   "Function call injection"
.486p
        .xlist
include ks386.inc
include callconv.inc
        .list

_TEXT   SEGMENT PARA PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

        page , 132
        subttl  "Function call injection"

        extrn   _DwPreFuncCall@4:near
        extrn   _PostFuncCall@4:near

;++
;
; SetupFuncCall
;
; Wraps all of the work needed to do an embedded function call
;
;--
cPublicProc _SetupFuncCall, 0
cPublicFpo 0,0
        push    eax
        push    ebp
        mov     ebp, esp
        sub     esp, 24
        mov     [ebp-4], eax
        mov     [ebp-8], ecx
        mov     [ebp-12], edx
        mov     [ebp-16], esi
        mov     [ebp-20], edi

; set up for our stack copy
        lea     eax, [ebp-24]
        push    eax
        call    _DwPreFuncCall@4
        mov     ecx, eax

; allocate our stack space
        sub     esp, eax
        mov     edx, [ebp-24]
        mov     edi, esp
        mov     esi, [edx]

; remember our return address
        test    ecx, ecx
        mov     eax, [eax+esi]
        mov     [ebp+4], eax

; if there's nothing to copy, then we won't actually copy the data or make the
; call
        je      Sfc10

; copy the stack data
        rep     movsb

; the address at TOS is the one we're going to call, so call it
        pop     eax
        call    eax
        mov     [ebp-4], eax

; clean up
Sfc10:
        push    [ebp-24]
        call    _PostFuncCall@4
        mov     eax, [ebp-4]
        mov     ecx, [ebp-8]
        mov     edx, [ebp-12]
        mov     esi, [ebp-16]
        mov     edi, [ebp-20]
        mov     esp, ebp
        pop     ebp
        ret

stdENDP _SetupFuncCall

_TEXT   ends
        end
