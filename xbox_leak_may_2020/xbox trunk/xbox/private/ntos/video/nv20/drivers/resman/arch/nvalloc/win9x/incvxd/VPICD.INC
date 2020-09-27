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

VPICD_STAT_IRET_PENDING	EQU	01H
VPICD_STAT_IRET_PENDING_BIT	EQU	0
VPICD_STAT_IN_SERVICE	EQU	02H
VPICD_STAT_IN_SERVICE_BIT	EQU	1
VPICD_STAT_PHYS_MASK	EQU	04H
VPICD_STAT_PHYS_MASK_BIT	EQU	2
VPICD_STAT_PHYS_IN_SERV	EQU	08H
VPICD_STAT_PHYS_IN_SERV_BIT	EQU	3
VPICD_STAT_VIRT_MASK	EQU	10H
VPICD_STAT_VIRT_MASK_BIT	EQU	4
VPICD_STAT_VIRT_REQ	EQU	20H
VPICD_STAT_VIRT_REQ_BIT	EQU	5
VPICD_STAT_PHYS_REQ	EQU	40H
VPICD_STAT_PHYS_REQ_BIT	EQU	6
VPICD_STAT_VIRT_DEV_REQ	EQU	80H
VPICD_STAT_VIRT_DEV_REQ_BIT	EQU	7
VPICD_STAT_GLOBAL	EQU	100H
VPICD_STAT_GLOBAL_BIT	EQU	8
VPICD_STAT_OWNED_BY_VM	EQU	200H
VPICD_STAT_OWNED_BY_VM_BIT	EQU	9
VPICD_OPT_READ_HW_IRR	EQU	01H
VPICD_OPT_READ_HW_IRR_BIT	EQU	0
VPICD_OPT_CAN_SHARE	EQU	02H
VPICD_OPT_CAN_SHARE_BIT	EQU	1
VPICD_OPT_REF_DATA	EQU	04H
VPICD_OPT_REF_DATA_BIT	EQU	2
VPICD_OPT_VIRT_INT_REJECT	EQU	10H
VPICD_OPT_VIRT_INT_REJECT_BIT	EQU	4
VPICD_OPT_SHARE_PMODE_ONLY	EQU	20H
VPICD_OPT_SHARE_PMODE_ONLY_BIT	EQU	5
VPICD_OPT_ALL	EQU	3FH
VPICD_FDO_NO_CONTENTION	EQU	10000H
VPICD_FDO_NO_CONTENTION_BIT	EQU	16
VPICD_FDO_FAVOR_FOCUS	EQU	20000H
VPICD_FDO_FAVOR_FOCUS_BIT	EQU	17

VPICD_IRQ_Descriptor	STRUC
VID_IRQ_Number	DW	?
VID_Options	DW	0
VID_Hw_Int_Proc	DD	?
VID_Virt_Int_Proc	DD	0
VID_EOI_Proc	DD	0
VID_Mask_Change_Proc	DD	0
VID_IRET_Proc	DD	0
VID_IRET_Time_Out	DD	500
VID_Hw_Int_Ref	DD	?
VPICD_IRQ_Descriptor	ENDS
Begin_Service_Table VPICD
VPICD_Service	 VPICD_Get_Version, VxD_LOCKED_CODE
VPICD_Service	 VPICD_Virtualize_IRQ, VxD_LOCKED_CODE
VPICD_Service	 VPICD_Set_Int_Request, VxD_LOCKED_CODE
VPICD_Service	 VPICD_Clear_Int_Request, VxD_LOCKED_CODE
VPICD_Service	 VPICD_Phys_EOI, VxD_LOCKED_CODE
VPICD_Service	 VPICD_Get_Complete_Status, VxD_LOCKED_CODE
VPICD_Service	 VPICD_Get_Status, VxD_LOCKED_CODE
VPICD_Service	 VPICD_Test_Phys_Request, VxD_LOCKED_CODE
VPICD_Service	 VPICD_Physically_Mask, VxD_LOCKED_CODE
VPICD_Service	 VPICD_Physically_Unmask, VxD_LOCKED_CODE
VPICD_Service	 VPICD_Set_Auto_Masking, VxD_LOCKED_CODE
VPICD_Service	 VPICD_Get_IRQ_Complete_Status, VxD_LOCKED_CODE
VPICD_Service	 VPICD_Convert_Handle_To_IRQ, VxD_LOCKED_CODE
VPICD_Service	 VPICD_Convert_IRQ_To_Int, VxD_LOCKED_CODE
VPICD_Service	 VPICD_Convert_Int_To_IRQ, VxD_LOCKED_CODE
VPICD_Service	 VPICD_Call_When_Hw_Int, VxD_LOCKED_CODE
VPICD_Service	 VPICD_Force_Default_Owner, VxD_LOCKED_CODE
VPICD_Service	 VPICD_Force_Default_Behavior, VxD_LOCKED_CODE
VPICD_Service	 VPICD_Auto_Mask_At_Inst_Swap
VPICD_Service	 VPICD_Begin_Inst_Page_Swap
VPICD_Service	 VPICD_End_Inst_Page_Swap
VPICD_Service	 VPICD_Virtual_EOI, VxD_LOCKED_CODE
VPICD_Service	 VPICD_Get_Virtualization_Count
VPICD_Service	 VPICD_Post_Sys_Critical_Init, LOCAL
VPICD_Service	 VPICD_VM_SlavePIC_Mask_Change, VxD_LOCKED_CODE
End_Service_Table VPICD
