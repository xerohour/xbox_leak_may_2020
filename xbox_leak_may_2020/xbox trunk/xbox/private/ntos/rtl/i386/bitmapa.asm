	title  "Bitmap Functions"
;++
;
; Copyright (c) 1999  Microsoft Corporation
;
; Module Name:
;
;    bitmapa.asm
;
; Abstract:
;
;    This module implements functions to perform bitmap operations.
;
; Author:
;
;    Tracy Sharpe (tracysh) 16-Dec-1999
;
; Environment:
;
;    User or Kernel mode.
;
; Revision History:
;
;--
.486p
	.xlist
include ks386.inc
include callconv.inc            ; calling convention macros
	.list

_TEXT   SEGMENT PARA PUBLIC 'CODE'
	ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

	PAGE
	SUBTTL "RtlFindFirstSetRightMember"
;++
;
; USHORT
; RtlFindFirstSetRightMember(
;    IN ULONG Set
;    )
;
; /*++
;
; Routine Description:
;
;    Searches the Set for the lest signficant set bit.
;
; Arguments:
;
;    (ecx) Set - 32-bit value to operate on.
;
; Return Value:
;
;    Bit position of the first set right member.
;
;--

cPublicFastCall RtlFindFirstSetRightMember  ,1
cPublicFpo 0, 0

    bsf     eax, ecx
	fstRET  RtlFindFirstSetRightMember

fstENDP RtlFindFirstSetRightMember

_TEXT   ends
	    end
