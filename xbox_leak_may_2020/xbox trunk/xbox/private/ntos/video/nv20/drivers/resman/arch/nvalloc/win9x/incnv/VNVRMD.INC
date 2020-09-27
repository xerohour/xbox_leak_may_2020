;********* Operating System Interface for NVidia Resource Manager **********;
;                                                                           ;
; Module: NVRM.INC                                                          ;
;       Header file for NVRM defines and service exports.                   ;
;                                                                           ;
;---------------------------------------------------------------------------;
;                                                                           ;
; History:                                                                  ;
;        David Schmenk (dschmenk)     11/19/93 - wrote it                   ;
;                                                                           ;
;*********************** Copyright 1993 NVidia, Inc. ***********************;

;
; Version numbers.
;

NVRM_Major_Version  EQU     01h
NVRM_Minor_Version  EQU     00h

;
; Device IDs.
;

NVRM_Device_ID      EQU     032DCh

;
; Init order.
;

NVRM_Init_Order     EQU     070000000h

;===========================================================================;
;                                                                           ;
;                     Virtual device services.                              ;
;                                                                           ;
;       The virtual device services are declared here, along with their     ;
;       cod segments.  The service entrypoints are declared with a preceding;
;       underscore, the service is a C based function with stack based      ;
;       parameters.  Otherwise, it is an assembly based function with       ;
;       register based parameters.                                          ;
;                                                                           ;
;===========================================================================;

Begin_Service_Table NVRM
        NVRM_Service    NVRM_Get_Version,   LOCAL
        NVRM_Service    NVRM_Enable_Vga,    LOCAL
        NVRM_Service    NVRM_Fixup_Vga,     LOCAL
        NVRM_Service    NVRM_Enable_Hires,  LOCAL
        NVRM_Service    NVRM_Set_Dpm_Level, LOCAL
        NVRM_Service    NVRM_DDC,           LOCAL
        NVRM_Service    NVRM_ServicePlace,  LOCAL
        NVRM_Service    NVRM_GetAddress,    LOCAL
        NVRM_Service    NVRM_AuNote,        LOCAL
End_Service_Table NVRM


