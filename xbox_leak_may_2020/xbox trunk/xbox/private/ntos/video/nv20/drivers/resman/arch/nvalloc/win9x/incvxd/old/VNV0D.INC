;******** Operating System Interface for NVidia Hardware Simulator  ********;
;                                                                           ;
; Module: NV0.INC                                                           ;
;       Header file for NV0 defines and service exports.                    ;
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

NV0_Major_Version  EQU     01h
NV0_Minor_Version  EQU     00h

;
; Device IDs.
;

NV0_Device_ID      EQU     032DBh

;
; Init order.
;

NV0_Init_Order     EQU     070000000h

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

Begin_Service_Table     NV0
        NV0_Service     _nv0GetVersion,         LOCAL
        NV0_Service     _nv0InstallHandlers,    LOCAL
End_Service_Table       NV0


