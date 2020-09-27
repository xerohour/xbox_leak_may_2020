        TITLE   "Memory Management Support Routines"
;++
;
;  Copyright (c) 2001  Microsoft Corporation
;
;  Module Name:
;
;     superio.asm
;
;  Abstract:
;
;     This module implements the routines to interact with the system super I/O
;     controller.
;
;  Environment:
;
;     Kernel mode only.
;
;--

        .586p
        .xlist
INCLUDE ks386.inc
INCLUDE callconv.inc
        .list

;
; Define the ports used to interact with the super I/O controller.
;

SMC_CONFIG_PORT                     EQU     02Eh
SMC_INDEX_PORT                      EQU     02Fh
SMC_DATA_PORT                       EQU     02Fh

;
; Define the values to enter and exit the configuration mode of the super I/O
; controller.
;

SMC_ENTER_CONFIGURATION_MODE        EQU     055h
SMC_EXIT_CONFIGURATION_MODE         EQU     0AAh

;
; Define the logical device numbers.
;

SMC_FLOPPY_DRIVE_CONTROLLER         EQU     000h
SMC_PARALLEL_PORT                   EQU     003h
SMC_SERIAL_PORT_1                   EQU     004h
SMC_SERIAL_PORT_2                   EQU     005h
SMC_KEYBOARD                        EQU     007h
SMC_GAME_PORT                       EQU     009h
SMC_PME                             EQU     00Ah
SMC_MPU_401                         EQU     00Bh
SMC_USB_HUB                         EQU     00Ch

;
; Define the global configuration registers.
;

SMC_LOGICAL_DEVICE_NUMBER           EQU     007h

SMC_DEVICE_ACTIVATE                 EQU     030h
SMC_DEVICE_PRIMARY_ADDRESS_HIGH     EQU     060h
SMC_DEVICE_PRIMARY_ADDRESS_LOW      EQU     061h

;++
;
;   IODELAY
;
;   Macro Description:
;
;       This macro simply does a jmp to next instruction to synchronize
;       IO port access.
;
;   Arguments:
;
;       None
;
;--

IODELAY         macro
ifdef NEC_98
        out     5fh,al              ; same effect with "stdcall IODelay8259"
else  ; NEC_98
        jmp     $+2
endif ; NEC_98
endm

_TEXT   SEGMENT DWORD PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

;++
;
; VOID
; DbgInitializeSuperIo(
;     VOID
;     )
;
; Routine Description:
;
;     This function initializes the super I/O controller.
;
; Arguments:
;
;     None.
;
; Return Value:
;
;     None.
;
;--
cPublicProc _DbgInitializeSuperIo, 0

;
; Place the the super I/O controller in configuration mode.
;

        mov     edx, SMC_CONFIG_PORT
        mov     al, SMC_ENTER_CONFIGURATION_MODE
        out     dx, al
        IODelay

;
; Select the first serial I/O port for configuration.
;

        mov     al, SMC_LOGICAL_DEVICE_NUMBER
        out     dx, al
        IODelay
        inc     edx
        mov     al, SMC_SERIAL_PORT_1
        out     dx, al
        IODelay
        dec     edx

;
; Activate the first serial I/O port at a base address of 0x03F8.
;

        mov     al, SMC_DEVICE_ACTIVATE
        out     dx, al
        IODelay
        inc     edx
        mov     al, 1
        out     dx, al
        IODelay
        dec     edx

        mov     al, SMC_DEVICE_PRIMARY_ADDRESS_HIGH
        out     dx, al
        IODelay
        inc     edx
        mov     al, 003h
        out     dx, al
        IODelay
        dec     edx

        mov     al, SMC_DEVICE_PRIMARY_ADDRESS_LOW
        out     dx, al
        IODelay
        inc     edx
        mov     al, 0F8h
        out     dx, al
        IODelay
        dec     edx

;
; Take the the super I/O controller out of configuration mode.
;

        mov     al, SMC_EXIT_CONFIGURATION_MODE
        out     dx, al

        stdRET  _DbgInitializeSuperIo

stdENDP _DbgInitializeSuperIo

_TEXT  ends

        END
