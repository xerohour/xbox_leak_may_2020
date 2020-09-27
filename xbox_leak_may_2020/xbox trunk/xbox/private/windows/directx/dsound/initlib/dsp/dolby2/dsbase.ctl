; ******************************************************************
;
; 	Unpublished work.  Copyright 2000 Dolby Laboratories Inc.
; 	All Rights Reserved.
;												
;	File:			dsBase.ctl
;												
;   Contents:		Base addresses for GE X-Box
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

;	p: memory space(s)

;;;; Note: Interrupt vectors are in ROM on nvidia system
;;;;symbol		INTERRUPTVECTORBASE		$000000		; Interrupt Table Base
;;;;symbol		SUPEREXECBASE			$000004		; Superexec Base

symbol		SUPEREXECBASE			$000000		; Superexec Base
symbol		LOADERBASE				$000180		; Loader Base - was $140
;symbol		LOADERTABLEBASE			$0002c0		; Loader Table Base - was $280
symbol		LOADERTABLEBASE			$000100		; Loader Table Base - was $280 (XXXXX temp until dolby gives us more space)
symbol		DOLBYEXECBASE			$000300		; Exec+Sub Base - was $2c0

;	x: memory space(s)

symbol		CONFIGBASE				$000bc0
symbol		STACKEXTNBASE			$000be0		; Stack extension Base
symbol		XMEMTOP					$000bff		; Last address in x:


;	Special Symbols for simulating X-Box system memory 
;	in unused, upper DSP memory (Temporary)

;	x: memory space(s)

symbol		SYSMEMTABLEBASE	 	 	$100000
symbol		SYSMEMPCMBASE	 		$200000
symbol		SYSMEMLTRTBASE	 	 	$300000
symbol		SYSMEMAC3BASE	   	 	$400000
symbol		SYSMEMCONFIGBASE		$500000
symbol		SYSMEMHEAPBASE		 	$600000

;	y: memory space(s)

symbol		SYSMEMROMBASE			$000800