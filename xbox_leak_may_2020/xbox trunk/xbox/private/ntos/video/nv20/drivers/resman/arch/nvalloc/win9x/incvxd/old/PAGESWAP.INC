;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1988-1990
;
;   Title:	PageSwap.Inc - Demand Paging Swap Device Services
;
;   Version:	1.00
;
;   Date:	18-Oct-1988
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   18-Oct-1988 RAL Original
;   19-Oct-1990 JEM Added Buffer Descriptor structure
;
;==============================================================================


Begin_Service_Table PageSwap

PageSwap_Service PageSwap_Get_Version, LOCAL
PageSwap_Service PageSwap_Test_Create, LOCAL
PageSwap_Service PageSwap_Create, LOCAL
PageSwap_Service PageSwap_Destroy, LOCAL
PageSwap_Service PageSwap_In, LOCAL
PageSwap_Service PageSwap_Out, LOCAL
PageSwap_Service PageSwap_Test_IO_Valid, LOCAL

End_Service_Table PageSwap

PS_Zero_Init_Mask	EQU	0001h
PS_Zero_Init_Bit	EQU	00h
PS_Fixed_Page_Mask	EQU	0008h
PS_Fixed_Page_Bit	EQU	03h
PS_First_Page_In_Mask	EQU	0020h
PS_First_Page_In_Bit	EQU	05h
PS_Dirty_Mask		EQU	0040h
PS_Dirty_Bit		EQU	06h
PS_Ever_Dirty_Mask	EQU	8000h
PS_Ever_Dirty_Bit	EQU	0Fh

PS_NoXchg_Mask		EQU	80000000h
PS_NoXchg_Bit		EQU	1Fh	; DO NOT xchg phys addrs on PageSwap_Out


; Buffer descriptor for asynchronous I/O

SIZE_PS_BD_RESERVED EQU 44

PageSwapBufferDesc	STRUC
PS_BD_Next		dd	?
PS_BD_List		dw	?
PS_BD_Cmd		db	?
PS_BD_Priority		db	?
PS_BD_Page_Number	dd	?
PS_BD_Buffer_Ptr	dd	?
PS_BD_File_Page 	dd	?
PS_BD_Call_Back 	dd	?
PS_BD_Reserved		db SIZE_PS_BD_RESERVED dup (?)
PageSwapBufferDesc	ENDS

; List values for PS_BD_List

PS_BDL_Free		EQU	1
PS_BDL_Pending_WB	EQU	2
PS_BDL_Submitted_RA	EQU	3
PS_BDL_Locked		EQU	4

PS_BDP_Invalid_Data	EQU	-1
