        title   "Thread start"
.486p
        .xlist
include ks386.inc
include callconv.inc
        .list

_TEXT   SEGMENT PARA PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

        page , 132
        subttl  "Thread start"

        extrn   _FStartThread@12:near

;++
;
; DmpThreadStartup
;
; Provided as the substitute thread system routine for xapi threads so that we
; can send the appropriate notifications
;
;--

cPublicProc _DmpThreadStartup, 2
cPublicFpo 0,0
        mov     ecx, [esp+4]            ; copy args
        mov     eax, [esp+8]
        push    eax                     ; make space for our data
        push    eax
        push    eax                     ; push the args again
        push    ecx
        lea     eax, [esp+8]
        push    eax
        call    _FStartThread@12
        pop     eax                     ; break if requested
        test    eax, eax
        jz      dts10
        int     3
dts10:
        pop     eax                     ; call the real start routine
        jmp     eax
; no return

stdENDP _DmpThreadStartup

;++
;
; CallOnStack
;
; Calls the requested function on a different stack
;
; Parameters:
;   [esp+4] - function to call
;   [esp+8] - new stack base
;   [esp+12] - pointer to argument block
;
;--

cPublicProc _CallOnStack, 3
        push    ebp                     ; build a frame link
        mov     ebp, esp
        push    esi
        push    edi
        mov     esi, [ebp+12]           ; prepare to switch stacks
        xor     eax, eax
        mov     edi, [esi]
        mov     [esi], eax              ; prevent recursion
        test    edi, edi                ; switch stacks only if non-NULL
        jz      cos10
        mov     esp, edi
cos10:
        mov     edx, [ebp+16]
        mov     eax, [ebp+8]
        push    edx
        call    eax                     ; call the new function
        mov     [esi], edi              ; restore the usable stack pointer
        lea     esp, [ebp-8]            ; unwind
        pop     edi
        pop     esi
        pop     ebp
        ret     12

stdENDP _CallOnStack

_TEXT   ends
        end
