        title  "ix ioaccess"
;++
;
; Copyright (c) 1989  Microsoft Corporation
;
; Module Name:
;
;    ixioacc.asm
;
; Abstract:
;
;    Procedures to correctly touch I/O registers.
;
; Author:
;
;    Bryan Willman (bryanwi) 16 May 1990
;
; Environment:
;
;    User or Kernel, although privledge (IOPL) may be required.
;
; Revision History:
;
;--

.386p
        .xlist
include hal386.inc
include callconv.inc                    ; calling convention macros
        .list

_TEXT   SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

;++
;
; I/O port space read and write functions.
;
;  These have to be actual functions on the 386, because we need
;  to use assembler, but cannot return a value if we inline it.
;
;  This set of functions manipulates I/O registers in PORT space.
;  (Uses x86 in and out instructions)
;
;  WARNING: Port addresses must always be in the range 0 to 64K, because
;           that's the range the hardware understands.
;
;--

;++
;
;   VOID
;   READ_PORT_BUFFER_UCHAR(
;       PUCHAR  Port,
;       PUCHAR  Buffer,
;       ULONG   Count
;       )
;
;   Arguments:
;       (esp+4) = Port
;       (esp+8) = Buffer address
;       (esp+12) = Count
;
;--
cPublicProc _READ_PORT_BUFFER_UCHAR ,3
cPublicFpo 3, 0

        mov     eax, edi                ; Save edi

        mov     edx,[esp+4]             ; (dx) = Port
        mov     edi,[esp+8]             ; (edi) = buffer
        mov     ecx,[esp+12]            ; (ecx) = transfer count
    rep insb
        mov     edi, eax
        stdRET    _READ_PORT_BUFFER_UCHAR

stdENDP _READ_PORT_BUFFER_UCHAR

;++
;
;   VOID
;   READ_PORT_BUFFER_USHORT(
;       PUSHORT Port,
;       PUSHORT Buffer,
;       ULONG   Count
;       )
;
;   Arguments:
;       (esp+4) = Port
;       (esp+8) = Buffer address
;       (esp+12) = Count
;
;--
cPublicProc _READ_PORT_BUFFER_USHORT ,3
cPublicFpo 3, 0

        mov     eax, edi                ; Save edi

        mov     edx,[esp+4]             ; (dx) = Port
        mov     edi,[esp+8]             ; (edi) = buffer
        mov     ecx,[esp+12]            ; (ecx) = transfer count
    rep insw
        mov     edi, eax
        stdRET    _READ_PORT_BUFFER_USHORT

stdENDP _READ_PORT_BUFFER_USHORT

;++
;
;   VOID
;   READ_PORT_BUFFER_ULONG(
;       PULONG  Port,
;       PULONG  Buffer,
;       ULONG   Count
;       )
;
;   Arguments:
;       (esp+4) = Port
;       (esp+8) = Buffer address
;       (esp+12) = Count
;
;--
cPublicProc _READ_PORT_BUFFER_ULONG ,3
cPublicFpo 3, 0

        mov     eax, edi                ; Save edi

        mov     edx,[esp+4]             ; (dx) = Port
        mov     edi,[esp+8]             ; (edi) = buffer
        mov     ecx,[esp+12]            ; (ecx) = transfer count
    rep insd
        mov     edi, eax
        stdRET    _READ_PORT_BUFFER_ULONG

stdENDP _READ_PORT_BUFFER_ULONG

;++
;
;   VOID
;   WRITE_PORT_BUFFER_UCHAR(
;       PUCHAR  Port,
;       PUCHAR  Buffer,
;       ULONG   Count
;       )
;
;   Arguments:
;       (esp+4) = Port
;       (esp+8) = Buffer address
;       (esp+12) = Count
;
;--
cPublicProc _WRITE_PORT_BUFFER_UCHAR ,3
cPublicFpo 3, 0

        mov     eax,esi                 ; Save esi
        mov     edx,[esp+4]             ; (dx) = Port
        mov     esi,[esp+8]             ; (esi) = buffer
        mov     ecx,[esp+12]            ; (ecx) = transfer count
    rep outsb
        mov     esi,eax
        stdRET    _WRITE_PORT_BUFFER_UCHAR

stdENDP _WRITE_PORT_BUFFER_UCHAR

;++
;
;   VOID
;   WRITE_PORT_BUFFER_USHORT(
;       PUSHORT Port,
;       PUSHORT Buffer,
;       ULONG   Count
;       )
;
;   Arguments:
;       (esp+4) = Port
;       (esp+8) = Buffer address
;       (esp+12) = Count
;
;--
cPublicProc _WRITE_PORT_BUFFER_USHORT ,3
cPublicFpo 3, 0

        mov     eax,esi                 ; Save esi
        mov     edx,[esp+4]             ; (dx) = Port
        mov     esi,[esp+8]             ; (esi) = buffer
        mov     ecx,[esp+12]            ; (ecx) = transfer count
    rep outsw
        mov     esi,eax
        stdRET    _WRITE_PORT_BUFFER_USHORT

stdENDP _WRITE_PORT_BUFFER_USHORT

;++
;
;   VOID
;   WRITE_PORT_BUFFER_ULONG(
;       PULONG  Port,
;       PULONG  Buffer,
;       ULONG   Count
;       )
;
;   Arguments:
;       (esp+4) = Port
;       (esp+8) = Buffer address
;       (esp+12) = Count
;
;--
cPublicProc _WRITE_PORT_BUFFER_ULONG ,3
cPublicFpo 3, 0

        mov     eax,esi                 ; Save esi
        mov     edx,[esp+4]             ; (dx) = Port
        mov     esi,[esp+8]             ; (esi) = buffer
        mov     ecx,[esp+12]            ; (ecx) = transfer count
    rep outsd
        mov     esi,eax
        stdRET    _WRITE_PORT_BUFFER_ULONG

stdENDP _WRITE_PORT_BUFFER_ULONG

_TEXT   ends

        end
