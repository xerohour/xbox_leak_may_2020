;*****************************************************************************
;Copyright (c) 2000  Microsoft Corporation
;
;Module Name:
;
;    GhEntry.asm
;
;Abstract:
;
;       The /Gh compiler flag inserts a call to __penter() at the beginning 
;   of each function. The catch is that it follows the __stdcall naming 
;   convention, but requires the __cdecl stack cleanup.
;       Thus we must write it in asm. And for my own sanity we just call a C
;   function.
;
;       The __pexit() function is not explicitly called by the /Gh flag. So
;   inside of __penter() we modify the return address to jump inside of our
;   __pexit, call our cleanup function, then jump back to where it was 
;   supposed to go in the first place. Note that __pexit should NEVER be
;   called, only jumped to and only with the proper setup via our __penter
;   method.
;
;Author:
;
;    jpoley
;
;Environment:
;
;    XBox
;
;Revision History:
;
;*****************************************************************************

.586p

    ; extern C++ worker functions
    EXTRN ?penterAddStats@@YGXXZ:PROC
    EXTRN ?penterCleanup@@YGIXZ:PROC

    ; function declarations
    PUBLIC __penter
    PUBLIC __pexit

_TEXT SEGMENT WORD PUBLIC 'CODE'


__penter PROC
    pushad                          ; push the registers
    call ?penterAddStats@@YGXXZ     ; call our worker function
    popad                           ; restore the registers
    ret                             ; all done
__penter ENDP


__pexit PROC
    sub esp, 4                  ; save room for the return address
    pushad                      ; we save the registers and flags cause we
    pushfd                      ; need to act like we were never here
    call ?penterCleanup@@YGIXZ  ; call the api that does all the work
    mov [esp+36], eax           ; save the return addr on the stack for 'ret'
    popfd                       ; restore flags
    popad                       ; and registers
    ret                         ; jump to the original return address
__pexit ENDP


_TEXT ENDS
END
