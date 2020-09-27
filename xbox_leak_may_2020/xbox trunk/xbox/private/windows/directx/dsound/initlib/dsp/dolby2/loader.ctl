; ******************************************************************
;
; 	Unpublished work.  Copyright 2000 Dolby Laboratories Inc.
; 	All Rights Reserved.
;												
;	File:			loader.ctl
;												
;   Contents:		Loader for Dolby Game Encoder
;
;	Author:			jmw
;											
;	Date Created:	9/12/2000
;											
;	History:
;
;		Rev 1.00	9/12/2000 12:45:30 PM		jmw
;	Created Linker Control File.
;
;
; ******************************************************************

;	Include Base addresses of all modules

include	"DsBase.ctl"

; ******************************************************************

;	no external x
;	no external y 
;	no external p

;	locate sections at desired location(s)

base	x:$0						; start address for x
reserve x:$0..$ffffff				; all reserved

base	y:$0						; start address for y
reserve	y:$0..$ffffff				; Can't use this for the loader

base	p:LOADERBASE				; start address for p:
reserve	p:$0..LOADERBASE-1			; can't use anything below Loader base
reserve	p:DOLBYEXECBASE..$ffffff	; Loader can't use this; for Exec+Sub

base	p(100):LOADERBASE			; Loader Base address
base	p(101):LOADERTABLEBASE		; Loader Table Base address