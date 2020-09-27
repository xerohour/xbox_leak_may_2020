;
;  (C) Copyright NVIDIA Corporation Inc., 1995,1996. All rights reserved.
;
;********* Operating System Interface for NVidia Resource Manager **********;
;                                                                           ;
; Module: VXDSTUB.ASM                                                       ;
;       These routines provide an operating system independent layer        ;
;       between the Resource Manager and the underlying OS APIs.            ;
;                                                                           ;
;---------------------------------------------------------------------------;
;                                                                           ;
; History:                                                                  ;
;        David Schmenk (dschmenk)     11/17/93 - wrote it                   ;
;        David Schmenk (dschmenk)     06/15/94 - rewrote for real RM        ;
;        JohnH                        10/3/97 - rewrote for MS tools--      ;
;                                     include DDB and service table here    ;
;*********************** Copyright 1993 NVidia, Inc. ***********************;

.386p

INCLUDE BASEDEF.INC
INCLUDE VMM.INC
INCLUDE VWIN32.INC
INCLUDE VPICD.INC
INCLUDE V86MMGR.INC
INCLUDE VNVRMD.INC
INCLUDE DEBUG.INC
;include         ..\..\..\..\..\sdk\nvidia\inc\rddapi.inc


VxD_LOCKED_DATA_SEG
PUBLIC  dwSrcLinearPageNum, dwDstLinearPageNum, dwLinearRegionSizeInPages, dwPageDirectoryBaseRegLinear
dwSrcLinearPageNum              DD      0
dwDstLinearPageNum              DD      0
dwLinearRegionSizeInPages       DD      0
dwPageDirectoryBaseRegLinear    DD      0

PUBLIC  dwPageTableEntry
dwPageTableEntry                DD      0
PUBLIC	dwGotNulPTE, dwNulPTE
dwGotNulPTE			DD	0
dwNulPTE			DD	0
VxD_LOCKED_DATA_ENDS


VxD_LOCKED_CODE_SEG

;   The Device Declartation Block

;Declare_Virtual_Device MACRO Name, Major_Ver, Minor_Ver, Ctrl_Proc, Device_Num, Init_Order, V86_Proc, PM_Proc, Reference_Data
;    LOCAL   V86_API_Offset, PM_API_Offset, Serv_Tab_Offset, Serv_Tab_Len, Ref_Data_Offset

Declare_Virtual_Device  NVACORE, NVRM_Major_Version, NVRM_Minor_Version, rmControlEntry, UNDEFINED_DEVICE_ID, UNDEFINED_INIT_ORDER, \
                        ,                    \
                        ,					\
                        ,

;---------------------------------------------------------------------------
;
;  VxD Control entrypoints.
;
;---------------------------------------------------------------------------
EXTRN   _rmAuControl : NEAR
public  rmControlEntry
BeginProc rmControlEntry
;       ebp points to the Client_Regiser_Structure
;       rmControl expects two args: msg, VMh
        push    ebx             ; VMh
        push    eax             ; msg
        call    _rmAuControl
        add     esp,2*4
        or      eax, eax
        stc
        jnz     @F
        clc
@@:     ret
EndProc rmControlEntry

VxD_LOCKED_CODE_ENDS

END
