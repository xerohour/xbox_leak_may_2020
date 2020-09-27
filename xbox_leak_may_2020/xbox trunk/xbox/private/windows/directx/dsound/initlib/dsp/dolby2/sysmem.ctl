; ******************************************************************
;
; 	Unpublished work.  Copyright 2000 Dolby Laboratories Inc.
; 	All Rights Reserved.
;												
;	File:			sysmem.ctl
;												
;   Contents:		System Memory for Dolby Game Encoder
;
;	Author:			jmw
;											
;	Date Created:	9/12/2000
;											
;	History:
;
;		Rev 1.00	9/12/2000 12:46:23 PM		jmw
;	Created Linker Control File.
;
;
; ******************************************************************

;	Include Base addresses of all/different modules

include	"DsBase.ctl"

; ******************************************************************

;	no external x
;	no external y 
;	no external p

;	locate sections at desired location(s)

base	x(100):SYSMEMTABLEBASE 			; System Memory Table Base
base	x(101):SYSMEMPCMBASE 			; System Memory PCM Base
base	x(102):SYSMEMLTRTBASE 			; System Memory Lt/Rt Base
base	x(103):SYSMEMAC3BASE 			; System Memory AC-3 Base
base	x(104):SYSMEMCONFIGBASE			; System memory Configuration Data Base
base	x(105):SYSMEMHEAPBASE 			; System Memory Heap Base

base	y(301):SYSMEMROMBASE			; System Memory ROM Base