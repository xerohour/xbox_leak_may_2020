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

ifndef	_VXDLDR_H
_VXDLDR_H	EQU	1
ifndef	Not_VxD
Begin_Service_Table	 VXDLDR, VxD
 VXDLDR_Service	 VXDLDR_GetVersion, LOCAL
 VXDLDR_Service	 VXDLDR_LoadDevice, LOCAL
 VXDLDR_Service	 VXDLDR_UnloadDevice, LOCAL
 VXDLDR_Service	 VXDLDR_DevInitSucceeded, LOCAL
 VXDLDR_Service	 VXDLDR_DevInitFailed, LOCAL
 VXDLDR_Service	 VXDLDR_GetDeviceList, LOCAL
 VXDLDR_Service    VXDLDR_UnloadMe, LOCAL
 VXDLDR_Service    _PELDR_LoadModule, VxD_PAGEABLE_CODE
 VXDLDR_Service    _PELDR_GetModuleHandle, VxD_PAGEABLE_CODE
 VXDLDR_Service    _PELDR_GetModuleUsage, VxD_PAGEABLE_CODE
 VXDLDR_Service    _PELDR_GetEntryPoint, VxD_PAGEABLE_CODE
 VXDLDR_Service    _PELDR_GetProcAddress, VxD_PAGEABLE_CODE
 VXDLDR_Service    _PELDR_AddExportTable, VxD_PAGEABLE_CODE
 VXDLDR_Service    _PELDR_RemoveExportTable, VxD_PAGEABLE_CODE
 VXDLDR_Service    _PELDR_FreeModule, VxD_PAGEABLE_CODE
 VXDLDR_Service    VXDLDR_Notify, LOCAL
 VXDLDR_Service    _PELDR_InitCompleted, VXD_PAGEABLE_CODE
 VXDLDR_Service    _PELDR_LoadModuleEx, VxD_PAGEABLE_CODE
End_Service_Table	 VXDLDR, VxD
endif
VXDLDR_INIT_DEVICE	EQU	000000001H
VXDLDR_ERR_OUT_OF_MEMORY	EQU	1
VXDLDR_ERR_IN_DOS	EQU	2
VXDLDR_ERR_FILE_OPEN_ERROR	EQU	3
VXDLDR_ERR_FILE_READ	EQU	4
VXDLDR_ERR_DUPLICATE_DEVICE	EQU	5
VXDLDR_ERR_BAD_DEVICE_FILE	EQU	6
VXDLDR_ERR_DEVICE_REFUSED	EQU	7
VXDLDR_ERR_NO_SUCH_DEVICE	EQU	8
VXDLDR_ERR_DEVICE_UNLOADABLE	EQU	9
VXDLDR_ERR_ALLOC_V86_AREA	EQU	10
VXDLDR_ERR_BAD_API_FUNCTION	EQU	11
VXDLDR_ERR_MAX	EQU	11
VXDLDR_NOTIFY_OBJECTUNLOAD	EQU	0
VXDLDR_NOTIFY_OBJECTLOAD	EQU	1
VXDLDR_APIFUNC_GETVERSION	EQU	0
VXDLDR_APIFUNC_LOADDEVICE	EQU	1
VXDLDR_APIFUNC_UNLOADDEVICE	EQU	2

ObjectInfo	STRUC
OI_LinearAddress	DD	?
OI_Size	DD	?
OI_ObjType	DD	?
OI_Resident	DD	?
ObjectInfo	ENDS

DeviceInfo	STRUC
DI_Next	DD	?
DI_LoadCount	DB	?
DI_DDB	DD	?
DI_DeviceID	DW	?
DI_ModuleName	DD	?
DI_Signature	DD	?
DI_ObjCount	DD	?
DI_ObjInfo	DD	?
DI_V86_API_CSIP	DD	?
DI_PM_API_CSIP	DD	?
DeviceInfo	ENDS
DI_Loaded	EQU	<DI_LoadCount>

_W32IoctlPkt	STRUC
W32IO_ErrorCode	DW	?
W32IO_DeviceID	DW	?
W32IO_ModuleName	DB	1 DUP (?)
_W32IoctlPkt	ENDS
ifndef	Not_VxD
endif
PELDR_ERR_NOERROR	EQU	0
PELDR_ERR_INVALIDHANDLE	EQU	-1
PELDR_ERR_MEM_NOMEMORY	EQU	-2
PELDR_ERR_FILE_UNABLETOOPEN	EQU	-3
PELDR_ERR_FILE_READERROR	EQU	-4
PELDR_ERR_IMAGE_INVALIDSIZE	EQU	-5
PELDR_ERR_IMAGE_INVALIDFORMAT	EQU	-6
PELDR_ERR_IMAGE_UNKNOWNFIXUP	EQU	-7
PELDR_ERR_IMAGE_INVALIDFIXUP	EQU	-8
PELDR_ERR_IMAGE_UNDEFINEDIMPORT	EQU	-9
PELDR_ERR_EXPORT_DUPLICATE	EQU	-10
PELDR_ERR_INIT_WASCOMPLETED	EQU	-11
PELDR_LOADFLAG_FREERESOURCES	EQU	00000001H
endif
