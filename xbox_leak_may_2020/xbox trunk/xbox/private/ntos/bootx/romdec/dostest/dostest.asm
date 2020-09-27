.586p

ENC_BUF_SIZE      EQU    6000h


ORIGIN            EQU    100h
                

; ==========================================================================

_TEXT   SEGMENT  PARA USE16 PUBLIC 'CODE'
        ASSUME  CS:_TEXT, DS:_TEXT, SS:_TEXT, ES:_TEXT
        ORG ORIGIN


        PUBLIC Start

Start:
        mov     sp, OFFSET StackBase

;
; open input file
;
        mov     ax, 3D00h
        lea     dx, InFileName
        int     21h
        mov     [FileHandle], ax

;
; read input file
;
        mov     ah, 3Fh
        mov     bx, [FileHandle]
        mov     cx, ENC_BUF_SIZE
        lea     dx, InBuffer
        int     21h

;
; close input file
;
        mov     ah, 3Eh        
        mov     bx, [FileHandle]
        int     21h


; ==========================================================================
;
; encrypt the buffer
;        
        mov     ax, ds
        mov     es, ax
          
        call    ZTimerOn
 
        call    RC4Encrypt

        call    ZTimerOff
        call	ZTimerReport


 
; ==========================================================================

;
; create output file
;
        mov     ah, 3Ch
        mov     cx, 0
        lea     dx, OutFileName
        int     21h

;
; open output file
;
        mov     ax, 3D01h
        lea     dx, OutFileName
        int     21h
        mov     [FileHandle], ax

;
; write output file
;
        mov     ah, 40h
        mov     bx, [FileHandle]
        mov     cx, ENC_BUF_SIZE
        lea     dx, OutBuffer
        int     21h
;
; close output file
;
        mov     ah, 3Eh        
        mov     bx, [FileHandle]
        int     21h


;
; DOS Exit 
;
Exit:
        mov     ah, 04Ch
        mov     al, 0
        int     21h


;
; encryption key
;
INCLUDE ENCKEY.INC



INCLUDE ..\I386\ENCRYPT.INC

INCLUDE ..\I386\ZENTIME.INC


_rc4KeyCtl  db 258 dup(?)


InFileName      db "in.bin", 0
FileHandle      dw -1

OutFileName     db "out.bin", 0
OutFileHandle   dw -1


        ALIGN   2

StackLimit      db 1024 dup (?)
StackBase       EQU $

ENC_INPUT_OFFSET  EQU    $
InBuffer        db  ENC_BUF_SIZE    DUP(0)
        
ENC_OUTPUT_OFFSET EQU    $
OutBuffer       db  ENC_BUF_SIZE    DUP(0)



_TEXT   ENDS
 
        END      Start
