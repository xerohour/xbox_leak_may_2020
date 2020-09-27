; ******************************************************************
;
; 	Unpublished work.  Copyright 2000 Dolby Laboratories Inc.
; 	All Rights Reserved.
;												
;	File:			suprexec.ctl
;												
;   Contents:		SuperExec for Dolby Game Encoder
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

base	x:$0								; start address for x
reserve	x:$0..CONFIGBASE-1					; off limits for SuperExec
reserve	x:XMEMTOP+1..$ffffff

base	y:$0								; start address for y
reserve	y:$0..$ffffff						; Can't use y for the SuperExec

base	p:$0							   	; SuperExec starts here
reserve	p:LOADERBASE..$ffffff				; all else reserved


base	p(200):INTERRUPTVECTORBASE			; Interrupt Vector Base - Counter p(200)
base	p(202):SUPEREXECBASE				; SuperExec Base - Counter p(202)
base	x(201):STACKEXTNBASE				; Stack extension Base - Counter x(201)
base	x(204):CONFIGBASE					; config data base - counter x(204)

