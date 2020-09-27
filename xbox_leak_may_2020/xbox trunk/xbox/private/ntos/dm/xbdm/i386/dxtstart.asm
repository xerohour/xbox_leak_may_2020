        title   "dxt entry call"
.486p
        .xlist
include ks386.inc
include callconv.inc
        .list

_TEXT   SEGMENT PARA PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

        page , 132
        subttl  "dxt entry call"

;++
;
; CallDxtEntry
;
; Call a debugger extension's entry point and keep the stack pointer safe
;
;--

cPublicProc _CallDxtEntry, 2
cPublicFpo 0,0
        xor     ecx, ecx
        mov     eax, [esp+4]
        mov     edx, [esp+8]
        push    ebp
        mov     ebp, esp
        push    ecx
        push    ecx
        push    edx
        call    eax
        mov     esp, ebp
        pop     ebp
        ret     8

stdENDP _CallDxtEntry

_TEXT   ends
        end
