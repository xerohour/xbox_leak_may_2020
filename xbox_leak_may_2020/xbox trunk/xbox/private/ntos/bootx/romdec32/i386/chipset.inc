;++
;
; Copyright (c) Microsoft Corporation
;
; Module Name:
;
;    chipset.inc
;
; Abstract:
;
;    This file contains defines that control which chipset romdec and
;    init table are built for
;
;--

IFNDEF RETAILXM3
;
; We want MCP_XMODE2 unless retail XM3 is being built
;
MCP_XMODE2 = 1

ENDIF

;
; Define the following, when building for MCP Rev B01 in XMODE3
; Otherwise, this should undefined
;
;MCP_REV_B01 = 1


IFDEF SYS_QT
;
; If we are building for QT which implies C03 (or higher), the following 
; should be defined For C02, it should not be defined
;
MCP_REV_C03 = 1

;
; If we are building for QT, we need to automatically define DVT6
;
SYS_DVT6 = 1

ENDIF

;
; If we are building retail, set the local retail define
;
IFDEF RETAILXM3

SYS_RETAIL = 1

ENDIF

