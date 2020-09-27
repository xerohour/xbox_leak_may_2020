        title   "Debug registers"
.486p
        .xlist
include ks386.inc
include callconv.inc
        .list

_TEXT   SEGMENT PARA PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

        page , 132
        subttl  "Debug registers"

;++
;
; SetDreg
;
; Set up dr0-dr3 and dr7 for hardware breakpoints
;
;--
cPublicProc _SetDreg, 5
cPublicFpo 0,0
; don't want to be bothered while we're setting this stuff up
        pushfd
        cli
        mov ecx, [esp+8]
; First set up a mask to clear out dr7
        shl ecx, 1
        mov eax, 0f0003h
        shl eax, cl
        not eax
        mov edx, dr7
        and eax, edx
        xor edx, edx
        mov dr7, eax
        mov dr6, edx
; If we're disabling the breakpoint, we're done now
        mov dl, [esp+24]
        shr ecx, 1
        test edx, edx
        jz sd10
; Set the appropriate dreg
        test ecx, ecx
        mov eax, [esp+12]
        jnz cl0
        mov dr0, eax
        jmp sd1
cl0:
        dec ecx
        jnz cl1
        mov dr1, eax
        jmp sd1
cl1:
        dec ecx
        jnz cl2
        mov dr2, eax
        jmp sd1
cl2:
        dec ecx
        jnz reg3
        mov dr3, eax
sd1:
; now construct the thing we actually shift into dr7
        xor edx, edx
        mov dl, [esp+20]
        shl edx, 2
        mov ecx, [esp+8]
        or dl, [esp+16]
        shl ecx, 1
        shl edx, 16
        or edx, 3
        mov eax, dr7
        shl edx, cl
        or edx, eax
        mov dr7, edx
; and now we're done
sd10:
        popfd
        ret 14h
reg3:
        int 3
        jmp sd10
stdENDP _SetDreg

cPublicFastCall DwExchangeDr6, 1
        mov eax, dr6
        mov dr6, ecx
        fstRET DwExchangeDr6
fstENDP DwExchangeDr6

_TEXT   ends
        end
