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

ifndef Not_VxD
Begin_Service_Table VWIN32
VWIN32_Service	 VWIN32_Get_Version, LOCAL
VWIN32_Service	 VWIN32_DIOCCompletionRoutine, LOCAL
VWIN32_Service	 _VWIN32_QueueUserApc
VWIN32_Service	 _VWIN32_Get_Thread_Context
VWIN32_Service	 _VWIN32_Set_Thread_Context
VWIN32_Service	 _VWIN32_CopyMem, LOCAL
VWIN32_Service	 _VWIN32_Npx_Exception
VWIN32_Service	 _VWIN32_Emulate_Npx
VWIN32_Service	 _VWIN32_CheckDelayedNpxTrap
VWIN32_Service	 VWIN32_EnterCrstR0
VWIN32_Service	 VWIN32_LeaveCrstR0
VWIN32_Service	 _VWIN32_FaultPopup
VWIN32_Service	 VWIN32_GetContextHandle
VWIN32_Service	 VWIN32_GetCurrentProcessHandle, LOCAL
VWIN32_Service	 _VWIN32_SetWin32Event
VWIN32_Service	 _VWIN32_PulseWin32Event
VWIN32_Service	 _VWIN32_ResetWin32Event
VWIN32_Service	 _VWIN32_WaitSingleObject
VWIN32_Service	 _VWIN32_WaitMultipleObjects
VWIN32_Service	 _VWIN32_CreateRing0Thread
VWIN32_Service	 _VWIN32_CloseVxDHandle
VWIN32_Service	 VWIN32_ActiveTimeBiasSet, LOCAL
VWIN32_Service	 VWIN32_GetCurrentDirectory, LOCAL
VWIN32_Service	 VWIN32_BlueScreenPopup
VWIN32_Service	 VWIN32_TerminateApp
VWIN32_Service	 _VWIN32_QueueKernelAPC
VWIN32_Service	 VWIN32_SysErrorBox
VWIN32_Service	 _VWIN32_IsClientWin32
VWIN32_Service	 VWIN32_IFSRIPWhenLev2Taken, LOCAL
End_Service_Table VWIN32
endif

vseb_s	STRUC
vseb_resp	DD	?
vseb_b3	DW	?
vseb_b2	DW	?
vseb_b1	DW	?
vseb_pszCaption	DD	?
vseb_pszText	DD	?
vseb_s	ENDS
KERNEL_APC_IGNORE_MC	EQU	00000001H
KERNEL_APC_STATIC	EQU	00000002H
KERNEL_APC_WAKE	EQU	00000004H

DIOCParams	STRUC
Internal1	DD	?
VMHandle	DD	?
Internal2	DD	?
dwIoControlCode	DD	?
lpvInBuffer	DD	?
cbInBuffer	DD	?
lpvOutBuffer	DD	?
cbOutBuffer	DD	?
lpcbBytesReturned	DD	?
lpoOverlapped	DD	?
hDevice	DD	?
tagProcess	DD	?
DIOCParams	ENDS
VWIN32_DIOC_GETVERSION	EQU	<DIOC_GETVERSION>
VWIN32_DIOC_DOS_IOCTL	EQU	1
VWIN32_DIOC_DOS_INT25	EQU	2
VWIN32_DIOC_DOS_INT26	EQU	3
VWIN32_DIOC_DOS_INT13	EQU	4
VWIN32_DIOC_SIMCTRLC	EQU	5
VWIN32_DIOC_CLOSEHANDLE	EQU	<DIOC_CLOSEHANDLE>

DIOCRegs	STRUC
reg_EBX	DD	?
reg_EDX	DD	?
reg_ECX	DD	?
reg_EAX	DD	?
reg_EDI	DD	?
reg_ESI	DD	?
reg_Flags	DD	?
DIOCRegs	ENDS
ifndef FILE_FLAG_OVERLAPPED

_OVERLAPPED	STRUC
O_Internal	DD	?
O_InternalHigh	DD	?
O_Offset	DD	?
O_OffsetHigh	DD	?
O_hEvent	DD	?
_OVERLAPPED	ENDS
endif
