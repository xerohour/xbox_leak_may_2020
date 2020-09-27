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

ifndef	_VMMREG_H
_VMMREG_H	EQU	1
MAX_VMM_REG_KEY_LEN	EQU	256
ifndef REG_SZ
REG_SZ	EQU	0001H
REG_BINARY	EQU	0003H
endif
ifndef HKEY_LOCAL_MACHINE
HKEY_CLASSES_ROOT	EQU	80000000H
HKEY_CURRENT_USER	EQU	80000001H
HKEY_LOCAL_MACHINE	EQU	80000002H
HKEY_USERS	EQU	80000003H
HKEY_PERFORMANCE_DATA	EQU	80000004H
HKEY_CURRENT_CONFIG	EQU	80000005H
HKEY_DYN_DATA	EQU	80000006H
endif
ifndef ERROR_FILE_NOT_FOUND
ERROR_FILE_NOT_FOUND	EQU	2
endif
ifndef ERROR_BADDB
ERROR_BADDB	EQU	1009
endif
ifndef ERROR_MORE_DATA
ERROR_MORE_DATA	EQU	234
endif
ifndef ERROR_BADKEY
ERROR_BADKEY	EQU	1010
endif
ifndef ERROR_CANTOPEN
ERROR_CANTOPEN	EQU	1011
endif
ifndef ERROR_CANTREAD
ERROR_CANTREAD	EQU	1012
ERROR_CANTWRITE	EQU	1013
endif
ifndef ERROR_REGISTRY_CORRUPT
ERROR_REGISTRY_CORRUPT	EQU	1015
ERROR_REGISTRY_IO_FAILED	EQU	1016
endif
ifndef ERROR_KEY_DELETED
ERROR_KEY_DELETED	EQU	1018
endif
ifndef ERROR_OUTOFMEMORY
ERROR_OUTOFMEMORY	EQU	14
endif
ifndef ERROR_INVALID_PARAMETER
ERROR_INVALID_PARAMETER	EQU	87
endif
ifndef ERROR_LOCK_FAILED
ERROR_LOCK_FAILED	EQU	167
endif
ifndef ERROR_NO_MORE_ITEMS
ERROR_NO_MORE_ITEMS	EQU	259
endif
ifndef  ERROR_SUCCESS
ERROR_SUCCESS	EQU	0
endif




LDR_RegOpenKey	Macro	hKey,OffSubKey,SegSubKey,OffphKey,SegphKey
	push	SegphKey
	push	OffphKey
	push	SegSubKey
	push	OffSubKey
	push	dword ptr hKey
	mov	ax,LDRSRV_RegOpenKey
	call	dword ptr [_ServiceEntry]
	add	sp,3*4
ENDM

LDR_RegCloseKey	Macro	hKey
	push	dword ptr hKey
	mov	ax,LDRSRV_RegCloseKey
	call	dword ptr [_ServiceEntry]
	add	sp,1*4
ENDM

LDR_RegCreateKey	Macro	hKey,OffSubKey,SegSubKey,OffphKey,SegphKey
	push	SegphKey
	push	OffphKey
	push	SegSubKey
	push	OffSubKey
	push	dword ptr hKey
	mov	ax,LDRSRV_RegCreateKey
	call	dword ptr [_ServiceEntry]
	add	sp,3*4
ENDM

LDR_RegDeleteKey	Macro	hKey,OffSubKey,SegSubKey
	push	SegSubKey
	push	OffSubKey
	push	dword ptr hKey
	mov	ax,LDRSRV_RegDeleteKey
	call	dword ptr [_ServiceEntry]
	add	sp,2*4
ENDM

LDR_RegEnumKey	Macro	hKey,iSubKey,OffszName,SegszName,BufLen
	push	dword ptr BufLen
	push	SegszName
	push	OffszName
	push	dword ptr iSubKey
	push	dword ptr hKey
	mov	ax,LDRSRV_RegEnumKey
	call	dword ptr [_ServiceEntry]
	add	sp,4*4
ENDM

LDR_RegQueryValue	Macro	hKey,OffSubKey,SegSubKey,OffValue,SegValue,OffcbValue,SegcbValue
	push	SegcbValue
	push	OffcbValue
	push	SegValue
	push	OffValue
	push	SegSubKey
	push	OffSubKey
	push	dword ptr hKey
	mov	ax,LDRSRV_RegQueryValue
	call	dword ptr [_ServiceEntry]
	add	sp,4*4
ENDM

LDR_RegSetValue	Macro	hKey,OffSubKey,SegSubKey,dwType,OffData,SegData,cbData
	push	dword ptr cbData
	push	SegData
	push	OffData
	push	dword ptr dwType
	push	SegSubKey
	push	OffSubKey
	push	dword ptr hKey
	mov	ax,LDRSRV_RegSetValue
	call	dword ptr [_ServiceEntry]
	add	sp,5*4
ENDM

LDR_RegDeleteValue	Macro	hKey,OffValue,SegValue
	push	SegValue
	push	OffValue
	push	dword ptr hKey
	mov	ax,LDRSRV_RegDeleteValue
	call	dword ptr [_ServiceEntry]
	add	sp,2*4
ENDM

LDR_RegEnumValue	Macro hKey,iValue,OffValue,SegValue,OffcbValue,SegcbValue,RegReserved,OffdwType,SegdwType,OffData,SegData,Offc
bData,SegcbData
	push	SegcbData
	push	OffcbData
	push	SegData
	push	OffData
	push	SegdwType
	push	OffdwType
	push	dword ptr RegReserved
	push	SegcbValue
	push	OffcbValue
	push	SegValue
	push	OffValue
	push	dword ptr iValue
	push	dword ptr hKey
	mov	ax,LDRSRV_RegEnumValue
	call	dword ptr [_ServiceEntry]
	add	sp,8*4
ENDM

LDR_RegQueryValueEx	Macro	hKey,OffValue,SegValue,RegReserved,OffdwType,SegdwType,OffData,SegData,OffcbData,SegcbData
	push	SegcbData
	push	OffcbData
	push	SegData
	push	OffData
	push	SegdwType
	push	OffdwType
	push	dword ptr RegReserved
	push	SegValue
	push	OffValue
	push	dword ptr hKey
	mov	ax,LDRSRV_RegQueryValueEx
	call	dword ptr [_ServiceEntry]
	add	sp,6*4
ENDM

LDR_RegSetValueEx	Macro	hKey,OffValue,SegValue,RegReserved,dwType,OffData,SegData,cbData
	push	dword ptr cbData
	push	SegData
	push	OffData
	push	dword ptr dwType
	push	dword ptr RegReserved
	push	SegValue
	push	OffValue
	push	dword ptr hKey
	mov	ax,LDRSRV_RegSetValueEx
	call	dword ptr [_ServiceEntry]
	add	sp,6*4
ENDM

LDR_RegFlushKey		Macro	hKey
	push	dword ptr hKey
	mov	ax,LDRSRV_RegFlushKey
	call	dword ptr [_ServiceEntry]
	add	sp,1*4
ENDM


endif
