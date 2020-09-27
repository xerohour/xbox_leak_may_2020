;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

option expr32

IFNDEF	basedef_inc
basedef_inc	EQU		1

; Begin of file ..\..\..\dev\ddk\inc\basedef.h
FALSE		EQU		0t
TRUE		EQU		1t
NULL		EQU		000000000h
_INT		TYPEDEF		SWORD

UINT		TYPEDEF		WORD

BOOL		TYPEDEF		SWORD

CHAR		TYPEDEF		SBYTE

UCHAR		TYPEDEF		BYTE

_SHORT		TYPEDEF		SWORD

USHORT		TYPEDEF		WORD

LONG		TYPEDEF		SDWORD

ULONG		TYPEDEF		DWORD

PVOID		TYPEDEF		PTR 

PPVOID		TYPEDEF		PTR PVOID

PINT		TYPEDEF		PTR _INT

PUINT		TYPEDEF		PTR UINT

PBYTE		TYPEDEF		PTR BYTE

PWORD		TYPEDEF		PTR WORD

PDWORD		TYPEDEF		PTR DWORD

PCHAR		TYPEDEF		PTR CHAR

PSHORT		TYPEDEF		PTR _SHORT

PLONG		TYPEDEF		PTR LONG

PUCHAR		TYPEDEF		PTR UCHAR

PUSHORT		TYPEDEF		PTR USHORT

PULONG		TYPEDEF		PTR ULONG

PBOOL		TYPEDEF		PTR BOOL

SZ		TYPEDEF		UCHAR

PSZ		TYPEDEF		PTR UCHAR

SZZ		TYPEDEF		UCHAR

PSZZ		TYPEDEF		PTR UCHAR

SEL		TYPEDEF		USHORT

PSEL		TYPEDEF		PTR SEL

PPHYS		TYPEDEF		ULONG

PFN		TYPEDEF		PTR 

PPFN		TYPEDEF		PTR PFN

HANDLE		TYPEDEF		PVOID

PHANDLE		TYPEDEF		PTR HANDLE

HTIMEOUT		TYPEDEF		ULONG

CMS		TYPEDEF		ULONG

; End of file ..\..\..\dev\ddk\inc\basedef.h

ENDIF

