        title   "Deferred notifications"
.486p
        .xlist
include ks386.inc
include callconv.inc
        .list

_TEXT   SEGMENT PARA PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

        page , 132
        subttl  "Deferred notifications"

        extrn   _g_ppdfnNext:DWORD
        extrn   _g_pdfnFirst:DWORD
        extrn   _ProcessDeferredNotifications@4:near

;++
;
; QueueDfn
;
; Queue a deferred notification on the global queue
;
;--

cPublicProc _QueueDfn, 1
cPublicFpo 0,0
        mov     eax, [esp+4]
        pushfd
        cli

; put this guy on the tail of the list
        mov     ecx, _g_ppdfnNext
        mov     [ecx], eax
        mov     _g_ppdfnNext, eax

        popfd
        ret     4

stdENDP _QueueDfn

cPublicProc _ProcessDfns, 0
cPublicFpo 0,0
        push    ebp
        mov     ebp, esp
        pushfd
        cli

; first, null-terminate the list
        xor     ecx, ecx
        mov     eax, _g_ppdfnNext
        mov     [eax], ecx

; figure out what work we're going to do
        mov     eax, _g_pdfnFirst
        push    eax

; now reset the list
        lea     eax, _g_pdfnFirst
        mov     _g_ppdfnNext, eax

; now we're ready to do the notifications
        sti
        call    _ProcessDeferredNotifications@4

; and we're done
        popfd
        mov     esp, ebp
        pop     ebp
        ret

stdENDP _ProcessDfns

_TEXT   ends
        end
