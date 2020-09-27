; ******************************************************************
;
; 	Unpublished work.  Copyright 2000 Dolby Laboratories Inc.
; 	All Rights Reserved.
;												
;	File:			sehelper.asm
;												
;   Contents:		Helper Functions for SuperExec
;					Only used for simulator version.
;
;	Author:			jmw
;											
;	Revision:		1.00
;											
;	History:
;
;		Rev 1.00	9/12/2000 3:49:54 PM		jmw
;	Created.
;
;
; ******************************************************************

	page	132,66,3,3
	opt		cre,loc,cc,cex,mex,mu,so

	title	'SuperExec Helper Fcns'

	section	SuperExecHelper
	
;**** include files ************************************************

	nolist
	include 'mot_equ.inc'
	list

;**** external references ******************************************

	xdef	CopyInFilesToSysMem
	xdef	Copy6ChInFileToSysMem

	xdef	CopySysMemToOutputFiles
	xdef	CopySysMemToAC3OutputFile

;**** equates ******************************************************

simulator_l_pcm_input		equ		$ef0000		; DSP chip locations for simulator
simulator_r_pcm_input		equ		$ef0001		; These will not be needed in NVIDIA
simulator_c_pcm_input		equ		$ef0002		; implementation
simulator_ls_pcm_input  	equ		$ef0003		;
simulator_rs_pcm_input  	equ		$ef0004		;

BLOCK_LEN					equ		256
AC3_BLOCK0_OFFSET			equ		(3072-1280-290)
AC3WORDSPERFRAME			equ		1280

;**** data *********************************************************

	mode rel
	org	p(202):

;*******************************************************************
;*
;*	Subroutine Name:	CopyInFilesToSysMem
;*																								
;*  Description:		Copies Input data to Simulated X-Box System memory
;*
;*	Input:
;*											
;*	Output:
;*											
;*	Modifies:
;*
;*	Locals:
;*
;*	Stack Space Used:
;*
;*******************************************************************
CopyInFilesToSysMem:

	move	#SYSMEMPCMBASE,r1
	
	dor		#BLOCK_LEN,l_loop
		move	x:simulator_l_pcm_input,a
		move	a,x:(r1)+
l_loop:

	dor		#BLOCK_LEN,c_loop
		move	x:simulator_c_pcm_input,a
		move	a,x:(r1)+
c_loop:

	dor		#BLOCK_LEN,r_loop
		move	x:simulator_r_pcm_input,a
		move	a,x:(r1)+
r_loop:

	dor		#BLOCK_LEN,ls_loop
		move	x:simulator_ls_pcm_input,a
		move	a,x:(r1)+
ls_loop:

	dor		#BLOCK_LEN,rs_loop
		move	x:simulator_rs_pcm_input,a
		move	a,x:(r1)+
rs_loop:

	rts


;*******************************************************************
;*
;*	Subroutine Name:	CopySysMemToOutputFiles
;*																								
;*  Description:		Copies simulated X-Box Sys Mem to Output debug files
;*
;*	Input:
;*											
;*	Output:
;*											
;*	Modifies:
;*
;*	Locals:
;*
;*	Stack Space Used:
;*
;*******************************************************************
CopySysMemToOutputFiles:

	move	#SYSMEMLTRTBASE,r0
	move	#SYSMEMLTRTBASE+BLOCK_LEN,r1

	dor		#BLOCK_LEN,outcopy
		movep	x:(r0)+,x:M_TX00
		movep	x:(r1)+,x:M_TX00
outcopy:

	rts


;*******************************************************************
;*
;*	Subroutine Name:	Copy6ChInFileToSysMem
;*																								
;*  Description:		Copies a single 6-channel block of Input data 
;*						to Simulated X-Box System memory
;*
;*	Input:
;*											
;*	Output:
;*											
;*	Modifies:
;*
;*	Locals:
;*
;*	Stack Space Used:
;*
;*******************************************************************
Copy6ChInFileToSysMem:

	move	#SYSMEMPCMBASE,r0
	move	#BLOCK_LEN,n1

	dor		#BLOCK_LEN,loop_6ch
		move	r0,r1
		movep	x:M_RX0,x:(r1)+n1
		movep	x:M_RX0,x:(r1)+n1
		movep	x:M_RX0,x:(r1)+n1
		movep	x:M_RX0,x:(r1)+n1
		movep	x:M_RX0,x:(r1)+n1
		movep	x:M_RX0,x:(r1)+n1
		move	(r0)+
loop_6ch:

	rts

;*******************************************************************
;*
;*	Subroutine Name:	CopySysMemToAC3OutputFile
;*																								
;*  Description:		Copies simulated X-Box Sys Mem to Output debug files
;*
;*	Input:				n0 - number of words to copy
;*											
;*	Output:
;*											
;*	Modifies:
;*
;*	Locals:
;*
;*	Stack Space Used:
;*
;*******************************************************************

CopySysMemToAC3OutputFile:

	move	#SYSMEMAC3BASE+AC3_BLOCK0_OFFSET,r0
	
	dor		#AC3WORDSPERFRAME,outcopyAC3
	movep	x:(r0)+,x:M_TX00
outcopyAC3:

	rts

;******************************************************************************

	endsec	; SuperExecHelper
	end
