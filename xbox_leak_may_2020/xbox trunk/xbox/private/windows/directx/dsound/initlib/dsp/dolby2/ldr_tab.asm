; ******************************************************************
;
; 	Unpublished work.  Copyright 2000 Dolby Laboratories Inc.
; 	All Rights Reserved.
;												
;	File:			ldr_tab.asm
;												
;   Contents:		Loader Table for System Memory addresses for X-Box
;
;	Author:			jmw
;											
;	Revision:		1.00
;											
;	History:
;
;		Rev 1.00	9/13/2000 8:30:42 AM		jmw
;	Created.
;
;
; ******************************************************************

	page	132,66,3,3
	opt		cre,loc,cc,cex,mex,mu,so

	title	'Loader Table'

	section	Loader_Table

;**** include files ***********

	nolist
	include 'ldr_sip.inc'
	list

;**** program code ************

	xdef	LdrTable

	org		p(101):					; Loader Table runtime counter is 101

LdrTable		ds		LDR_TABLE_SIZE

; ******************************************************************

	endsec		; Loader_Table

	end
