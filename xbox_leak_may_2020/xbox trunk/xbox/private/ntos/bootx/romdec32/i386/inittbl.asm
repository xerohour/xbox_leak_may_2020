;++
;
;  Copyright (c) Microsoft Corporation. All rights reserved
;
;  Module Name:
;
;    inittbl.asm
;
;  Abstract:
;
;    This module contains table of commands used by South Bridge to initialize
;    hardware and test memory during boot startup.  For definition of each
;    command, please see initcode.inc and command.inc
;
;--
        INCLUDE chipset.inc

;
; To build the init table for old A02 (without M7, fansink) DVT3s, DVT2s and 
; DVT1, define INITTABLE_OLDA02.  To build the init table for fullspeed systems, 
; this should undefined.  We are currently building two init tables:
;
;
;   Init Table            Ver    Defines
;   --------------------- ------ ----------------------------------------------------------
;   DVT4/5                0x46   SYS_DVT4  
;
;   DVT6                  0x60   SYS_DVT6 (SYS_DVT4 undefined)
;
;   QT                    0x70   SYS_QT (automatically defines SYS_DVT6)
;

        INCLUDE nv2ainit.inc

        END

