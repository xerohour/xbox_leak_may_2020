;==============================================================================
;
; Copyright (C) 1998, Nvidia Corporation
;
; File:         rddapi.inc
;
; Purpose:      This file holds the interface description for both
;               the minivdd and the resman's Register Display Driver
;               API. Register Display Driver is a private API between
;               16bit land and the driver components.
;==============================================================================

; To call the API, set:
; eax = VDD_REGISTER_DISPLAY_DRIVER_INFO  (defined in minivdd.inc)
; ebx = system VM handle
; ecx = one of the equates below
;
; Other register settings depend upon the value in ecx.

; These services are in the RegisterDisplayDriver function in the
; resman. They are services needed by the hardware specific portion
; of the display driver.

MVDD_GET_LOGDEV_SHARED_MEMORY                   EQU     8764FFFFH
MVDD_ALLOC_LINEAR_ADDRESS_SPACE                 EQU     87650000H
MVDD_FREE_LINEAR_ADDRESS_SPACE                  EQU     87650001H
MVDD_COPY_PTES_FOR_LINEAR_RANGE                 EQU     87650002H

; These services are in the RegisterDisplayDriver function in the
; minivdd. They are services needed by the generic portion of the
; display driver.
MVDD_SET_LOCK_AND_UNLOCK_OFFSCREEN_FUNCS        EQU     87650003H
MVDD_ALLOC_MEMORY_BLOCK                         EQU     87650004H
MVDD_FREE_MEMORY_BLOCK                          EQU     87650005H
MVDD_REENUMERATE_DEVNODE                        EQU     87650006H


